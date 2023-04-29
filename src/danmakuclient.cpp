#include "danmakuclient.h"
#include "livepackage.h"
#include "zlibhelper.h"
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimerEvent>
#include <QUrl>
#include <QVector>
#include <QWebSocket>
#include <QtEndian>
#include <QtGlobal>
#include <chrono>

using namespace Qt::Literals::StringLiterals;

DanmakuClient::DanmakuClient(QObject *parent)
    : QObject(parent),
      webSocket_(new QWebSocket),
      manager_(new QNetworkAccessManager(this)),
      timerid_(),
      roomid_(),
      realRoomid_()
{
    webSocket_->setParent(this);
    connect(webSocket_, &QWebSocket::connected, this, [this]() {
        emit connected();
        qDebug() << u"已连接房间号:"_s << roomid_;
        qDebug() << u"使用 API 获取房间号:"_s << realRoomid_;
        webSocket_->sendBinaryMessage(LivePackage::makeEnterRoomPackage(realRoomid_).toByteArray());
        using namespace std::chrono_literals;
        timerid_ = this->startTimer(30s);
        if (timerid_ == 0) qDebug() << u"无法设置计时器"_s;
    });
    connect(webSocket_, &QWebSocket::disconnected, this, [this]() {
        emit disconnected();
        if (timerid_ != 0) this->killTimer(timerid_);
        timerid_ = 0;
    });
    connect(webSocket_, &QWebSocket::binaryMessageReceived, this, &DanmakuClient::OnMessageReceived);
    connect(manager_, &QNetworkAccessManager::finished, this, [](QNetworkReply *reply) {
        reply->deleteLater();
    });
}

int DanmakuClient::roomid() const { return roomid_; }

void DanmakuClient::listen(int roomid)
{
    Q_ASSERT(roomid > 0);
    qDebug() << u"监听房间号:"_s << roomid;
    QNetworkRequest request;
    request.setUrl(QUrl(u"https://api.live.bilibili.com/room/v1/Room/room_init?id=%1"_s.arg(roomid)));
    QNetworkReply *reply = manager_->get(request);
    // 我也不知道为什么, 在这里 Qt 文档指示也是先调用 get() 再进行消息的连接
    // 应该不可能发生消息已经结束但是还没有连接成功吧?
    connect(reply, &QNetworkReply::finished, this, [this, roomid, reply]() {
        do {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << __FILE__ << __LINE__ << reply->errorString();
                break;
            }
            QJsonParseError error;
            QJsonDocument   json = QJsonDocument::fromJson(reply->readAll(), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug() << __FILE__ << __LINE__ << error.errorString();
                break;
            }
            if ((realRoomid_ = json["data"_L1]["room_id"_L1].toInt(0)) != 0) {
                webSocket_->open(QUrl("wss://broadcastlv.chat.bilibili.com/sub"_L1));
                roomid_ = roomid;
                emit listenChanged(roomid_, roomid);
            }
        } while (false);
    });
}

void DanmakuClient::stop()
{
    webSocket_->close();
    roomid_     = 0;
    realRoomid_ = 0;
}

void DanmakuClient::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerid_) this->heartbeat();
    return QObject::timerEvent(event);
}

void DanmakuClient::heartbeat()
{
    qDebug() << u"准备发送心跳"_s;
    webSocket_->sendBinaryMessage(LivePackage::makeHeartbeatPackage().toByteArray());
}

void DanmakuClient::OnMessageReceived(const QByteArray &message)
{
    LivePackage package = LivePackage::fromByteArray(message);
    switch (package.operation) {
    case LivePackage::Operation::HEARTBEAT_RESPONSE:
        qDebug() << u"心跳响应"_s;
        Q_ASSERT(package.body.size() == sizeof(quint32));
        if (package.body.size() != sizeof(quint32)) {
            qWarning() << u"包大小错误"_s << __FILE__ << __LINE__;
            break;
        }
        qDebug() << "当前人气:" << qFromBigEndian<quint32>(package.body.constData());
        emit popularityChanged(qFromBigEndian<quint32>(package.body.constData()));
        break;
    case LivePackage::Operation::ENTER_ROOM_RESPONSE:
        qDebug() << u"进入房间响应"_s;
        break;
    case LivePackage::Operation::NOTIFICATION: {
        switch (package.protocolVersion) {
        case LivePackage::ProtocolVersion::JSON_DOCUMENT: {
            QJsonParseError error;
            QJsonDocument   json = QJsonDocument::fromJson(package.body, &error);
            if (error.error != QJsonParseError::NoError) {
                qWarning() << u"JSON 格式化错误"_s << __FILE__ << __LINE__ << error.errorString();
                break;
            }
            qDebug() << json;
            Q_ASSERT(json.isObject());
            emit messageReceived(json.object());
            {
                QString cmd = json["cmd"_L1].toString();
                if (cmd.startsWith("WATCHED_CHANGE"_L1)) {
                    emit watchedChanged(json["data"_L1]["num"_L1].toInt());
                } else if (cmd.startsWith("SEND_GIFT"_L1)) {
                    const QJsonValue jsonData = json["data"_L1];
                    const int        price    = jsonData["price"_L1].toInt(); // 单个礼物的价格
                    const int        num      = jsonData["num"_L1].toInt();   // 礼物个数
                    emit giftReceived(num * price);
                }
            }
            break;
        }
        case LivePackage::ProtocolVersion::POPULARITY:
            Q_ASSERT(package.body.size() == sizeof(quint32));
            if (package.body.size() != sizeof(quint32)) {
                qWarning() << u"包大小错误"_s << __FILE__ << __LINE__;
                break;
            }
            qDebug() << "当前人气:" << qFromBigEndian<quint32>(package.body.constData());
            emit popularityChanged(qFromBigEndian<quint32>(package.body.constData()));
            break;
        case LivePackage::ProtocolVersion::ZLIB_COMPRESSED_BUFFER: {
            QByteArray ucBody = zlib_uncompress(package.body);
            for (qsizetype pos = 0; pos < ucBody.size();) {
                const char *start       = ucBody.constData() + pos;
                quint32     packageSize = qFromBigEndian<quint32>(start);
                Q_ASSERT(pos + packageSize <= ucBody.size());
                this->OnMessageReceived(QByteArray(start, packageSize));
                pos += packageSize;
            }
            break;
        }
        case LivePackage::ProtocolVersion::BROTLI_COMPRESSED_BUFFER:
            qWarning() << u"暂不支持的类型"_s;
            break;
        default:
            qWarning() << u"未知协议类型"_s;
            break;
        }
        break;
    }
    default:
        qDebug() << u"未知消息类型"_s;
    }
}
