#include "danmakuclient.h"
#include "zlibhelper.h"
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimerEvent>
#include <QUrl>
#include <QWebSocket>
#include <QtEndian>
#include <QtGlobal>
#include <chrono>

DanmakuClient::DanmakuClient(QObject *parent)
    : QObject(parent),
      webSocket_(new QWebSocket)
{
    webSocket_->setParent(this);
    connect(webSocket_, &QWebSocket::connected, this, [this]() {
        emit connected();
        qDebug() << "Connected:" << roomid_;
        QByteArray  msg(16, '\0');
        QJsonObject json;
        json["uid"]       = 0;
        json["roomid"]    = roomid_;
        json["protover"]  = 1;
        json["platform"]  = "web";
        json["clientver"] = "1.4.0";
        msg.append(QJsonDocument(json).toJson(QJsonDocument::Compact));
        char *a = msg.data();
        qToBigEndian<quint32>(msg.size(), a);            // 包总大小
        qToBigEndian<quint16>(16, a += sizeof(quint32)); // 包头大小
        qToBigEndian<quint16>(1, a += sizeof(quint16));  // 协议版本
        qToBigEndian<quint32>(7, a += sizeof(quint16));  // 操作码(认证并加入房间)
        qToBigEndian<quint32>(1, a += sizeof(quint32));  // ?
        webSocket_->sendBinaryMessage(msg);
        using namespace std::chrono_literals;
        timerid_ = startTimer(30s);
        if (timerid_ == 0) qDebug() << "Unable to set timer.";
    });
    connect(webSocket_, &QWebSocket::disconnected, this, [this]() {
        emit disconnected();
        if (timerid_ != 0) killTimer(timerid_);
        timerid_ = 0;
    });
    connect(webSocket_, &QWebSocket::binaryMessageReceived, this, [this](const QByteArray &data) {
        const char *a             = data.constData();
        quint32     packageLength = qFromBigEndian<quint32>(a);
        quint16     headerLength  = qFromBigEndian<quint16>(a += sizeof(quint32));
        Q_ASSERT(headerLength == 16);
        quint16 protocolVersion = qFromBigEndian<quint16>(a += sizeof(quint16));
        quint32 operation       = qFromBigEndian<quint32>(a += sizeof(quint16));
        quint32 seq             = qFromBigEndian<quint32>(a += sizeof(quint32));
        a += sizeof(quint32);
        // clang-format off
        qDebug() << "data.size:"       << data.size()
                 << "packageLength:"   << packageLength
                 << "headerLength:"    << headerLength
                 << "protocolVersion:" << protocolVersion
                 << "operation:"       << operation
                 << "seq:"             << seq
                 << "data:"            << data;
        // clang-format on
        switch (operation) {
        case HeartbeatResponse: qDebug() << "Heartbeat Response"; break;
        case ListenResponse: qDebug() << "Listen Response"; break;
        case InfoNotification:
            qDebug() << "Info Notification";
            if (protocolVersion == 2) {
                bool             ok;
                const QByteArray ucdata = zlib_uncompress(data.right(data.size() - headerLength), &ok);
                // ucdata 有多条数据, 根据每个头去取数据即可
                if (!ok) break;
                qDebug() << "ucdata.size:" << ucdata.size();
                const char *b = ucdata.constData();
                for (int s = 0, n = ucdata.size(); s < n;) {
                    quint32 subpackageLength = qFromBigEndian<quint32>(b);
                    quint16 subheaderLength  = qFromBigEndian<quint16>(b += sizeof(quint32));
                    Q_ASSERT(subheaderLength == 16);
                    quint16 subprotocolVersion = qFromBigEndian<quint16>(b += sizeof(quint16));
                    quint32 suboperation       = qFromBigEndian<quint32>(b += sizeof(quint16));
                    quint32 subseq             = qFromBigEndian<quint32>(b += sizeof(quint32));
                    b += sizeof(quint32);
                    qDebug() << QString::fromUtf8(b, subpackageLength - subheaderLength);
                    QJsonParseError error;
                    QJsonDocument   json = QJsonDocument::fromJson(QByteArray(b, subpackageLength - subheaderLength), &error);
                    if (error.error != QJsonParseError::NoError) {
                        qWarning() << error.errorString();
                        continue;
                    }
                    Q_ASSERT(json.isObject());
                    QJsonObject obj = json.object();
                    emit receivedJson(subprotocolVersion, suboperation, subseq, obj);
                    if (obj["cmd"] == "DANMU_MSG") emit receivedDanmaku(obj);
                    s += subpackageLength;
                    b += subpackageLength - subheaderLength;
                }
            } else {
                QJsonParseError error;
                QJsonDocument   json = QJsonDocument::fromJson(data.right(data.size() - headerLength), &error);
                if (error.error != QJsonParseError::NoError) {
                    qWarning() << error.errorString();
                    break;
                }
                Q_ASSERT(json.isObject());
                QJsonObject obj = json.object();
                emit receivedJson(protocolVersion, operation, seq, obj);
                if (obj["cmd"] == "DANMU_MSG") emit receivedDanmaku(obj);
            }
            break;
        default: qDebug() << "Unknown operation:" << operation; break;
        }
    });
}

void DanmakuClient::listen(int roomid)
{
    Q_ASSERT(roomid > 0);
    qDebug() << "Listen" << roomid;
    roomid_ = roomid;
    webSocket_->open(QUrl("wss://broadcastlv.chat.bilibili.com/sub"));
}

void DanmakuClient::stop()
{
    webSocket_->close();
    roomid_ = 0;
}

void DanmakuClient::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerid_) heartbeat();
    return QObject::timerEvent(event);
}

void DanmakuClient::heartbeat(quint32 seq)
{
    qDebug() << "Heartbeat";
    QByteArray msg(16, '\0');
    char      *a = msg.data();
    qToBigEndian<quint32>(0, a);                      // 包总大小
    qToBigEndian<quint16>(16, a += sizeof(quint32));  // 包头大小
    qToBigEndian<quint16>(1, a += sizeof(quint16));   // 协议版本
    qToBigEndian<quint32>(2, a += sizeof(quint16));   // 操作码(心跳)
    qToBigEndian<quint32>(seq, a += sizeof(quint32)); // ?
    webSocket_->sendBinaryMessage(msg);
}
