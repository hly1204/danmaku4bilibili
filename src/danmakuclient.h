#ifndef DANMAKU_CLIENT_H
#define DANMAKU_CLIENT_H

#include <QJsonObject>
#include <QObject>

// QT
class QWebSocket;
class QNetworkAccessManager;

class DanmakuClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int roomid READ roomid WRITE listen)

public:
    enum MessageType : int {
        DANMU_MSG,                     ///< 弹幕消息
        WELCOME_GUARD,                 ///< 欢迎老爷
        ENTRY_EFFECT,                  ///< 舰长进入房间
        WELCOME,                       ///< 用户进入房间
        SUPER_CHAT_MESSAGE_JPN,        ///< 日语 SC
        SUPER_CHAT,                    ///< SC
        SEND_GIFT,                     ///< 投喂礼物
        COMBO_SEND,                    ///< 连击礼物
        ANCHOR_LOT_START,              ///< 天选之人信息开始
        ANCHOR_LOT_END,                ///< 天选之人信息结束
        ANCHOR_LOT_AWARD,              ///< 天选之人获奖信息
        GUARD_BUY,                     ///< 上舰长
        USER_TOAST_MSG,                ///< 续费舰长
        NOTICE_MSG,                    ///< 本房间续费了舰长 (区别与自动续费?)
        ACTIVITY_BANNER_UPDATE_V2,     ///< 小时榜变动
        ROOM_REAL_TIME_MESSAGE_UPDATE, ///< 粉丝关注变动
        WATCHED_CHANGE,                ///< 观看人数变动
    };

    explicit DanmakuClient(QObject *parent = nullptr);

    int roomid() const;

public slots:
    /// \brief 监听房间号(最多只允许调用一次)
    void listen(int roomid);
    /// \brief 停止监听
    void stop();

signals:
    void messageReceived(const QJsonObject &json);
    void giftReceived(int gold);
    void popularityChanged(quint32 popularity);
    void watchedChanged(quint32 watched);
    void listenChanged(int oldroomid, int newroomid);
    void connected();
    void disconnected();

protected:
    void timerEvent(QTimerEvent *event) override;

private slots:
    void heartbeat();
    void OnMessageReceived(const QByteArray &message);

private:
    Q_DISABLE_COPY_MOVE(DanmakuClient)

private:
    QWebSocket            *webSocket_;
    QNetworkAccessManager *manager_;
    int                    timerid_;
    int                    roomid_;
};

#endif
