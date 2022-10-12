#ifndef DANMAKU_CLIENT_H
#define DANMAKU_CLIENT_H

#include <QObject>

class QWebSocket;

class DanmakuClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(DanmakuClient)

public:
    enum Operation : quint32 { HeartbeatResponse = 3,
                               InfoNotification  = 5,
                               ListenResponse    = 8 };

    explicit DanmakuClient(QObject *parent = nullptr);

public slots:
    /// \brief 监听房间号(最多只允许调用一次)
    void listen(int roomid);
    void stop();

signals:
    void receivedJson(quint16 protocolVersion, quint32 operation, quint32 seq, const QJsonValue &json);
    void receivedDanmaku(const QJsonValue &json);
    void connected();
    void disconnected();

protected:
    void timerEvent(QTimerEvent *event) override;

private slots:
    void heartbeat(quint32 seq = 1);

private:
    QWebSocket *webSocket_ = {};
    int         timerid_   = {};
    int         roomid_    = {};
};

#endif
