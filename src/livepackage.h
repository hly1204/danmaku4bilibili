#ifndef LIVE_PACKAGE_H
#define LIVE_PACKAGE_H

#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtEndian>
#include <QtGlobal>

class LivePackage
{
public:
    quint32    packageSize;     ///< 数据包长度
    quint16    headerSize;      ///< 数据包头部长度 (固定长度见枚举类型)
    quint16    protocolVersion; ///< 协议版本
    quint32    operation;       ///< 操作类型
    quint32    sequenceID;      ///< 数据包头部长度 (固定长度见枚举类型)
    QByteArray body;            ///< 包主体

    enum Misc {
        HEADER_SIZE              = 16, ///< 数据包头部长度
        PROTOCOL_VERSION_REQUEST = 1,  ///< 请求协议版本
        SEQUENCE_ID              = 1,  ///< 固定为 1
    };

    /// \brief 协议版本
    enum ProtocolVersion : quint16 {
        JSON_DOCUMENT            = 0, ///< JSON 文本
        POPULARITY               = 1, ///< 大端的 32 位整型, 表示人气
        ZLIB_COMPRESSED_BUFFER   = 2, ///< 使用 zlib 的 compress 函数压缩后的二进制数组
        BROTLI_COMPRESSED_BUFFER = 3, ///< 使用 brotli 压缩后的二进制数组 (暂时无法处理)
    };

    /// \brief 操作类型
    enum Operation : quint32 {
        HEARTBEAT           = 2, ///< 心跳包, 不发送则 70 秒后断开, 一般 30 秒发送一次
        HEARTBEAT_RESPONSE  = 3, ///< 心跳包响应
        NOTIFICATION        = 5, ///< 弹幕广播等一切通知
        ENTER_ROOM          = 7, ///< 进入房间, WebSocket 连接成功后发送的第一条必须为进入房间的信息
        ENTER_ROOM_RESPONSE = 8, ///< 进入房价的响应
    };

    /// \brief 消息类型
    enum Messgae : quint32 {};

    /// \brief 将包转换为二进制数组
    /// \details 其中包的长度和头部长度会自动设置, 不会使用当前的值
    /// \param[out] ok 若 \c ok 非空, 则将转换结果放入其中, 否则将返回空数组
    /// \returns 二进制数组
    QByteArray toByteArray(bool *ok = nullptr) const
    {
        QByteArray header(HEADER_SIZE, '\0');
        Q_ASSERT(headerSize == HEADER_SIZE);
        char *a = header.data();
        qToBigEndian<quint32>(body.size() + HEADER_SIZE, a);
        qToBigEndian<quint16>(HEADER_SIZE, a += sizeof(quint32));
        qToBigEndian<quint16>(protocolVersion, a += sizeof(quint16));
        qToBigEndian<quint32>(operation, a += sizeof(quint16));
        qToBigEndian<quint32>(SEQUENCE_ID, a + sizeof(quint32));
        return header.append(body);
    }

    /// \brief 将二进制数组转换为包
    /// \param[in] source 源
    /// \param[out] ok 若 \c ok 非空, 则将转换结果放入其中, 否则将返回空数组
    /// \returns 包
    static inline LivePackage fromByteArray(const QByteArray &source, bool *ok = nullptr)
    {
        LivePackage package;
        do {
            const char *a = source.constData();
            if (source.size() < HEADER_SIZE || static_cast<qsizetype>(package.packageSize = qFromBigEndian<quint32>(a)) != source.size()) {
                if (ok != nullptr) *ok = false;
                break;
            }
            package.headerSize      = qFromBigEndian<quint16>(a += sizeof(quint32));
            package.protocolVersion = qFromBigEndian<quint16>(a += sizeof(quint16));
            package.operation       = qFromBigEndian<quint32>(a += sizeof(quint16));
            package.sequenceID      = qFromBigEndian<quint32>(a += sizeof(quint32));
            using namespace Qt::Literals::StringLiterals;
            if (package.headerSize != HEADER_SIZE) qWarning() << u"包头长度错误"_s << __FILE__ << __LINE__;
            if (ok != nullptr) *ok = (package.headerSize == HEADER_SIZE);
            package.body = source.sliced(HEADER_SIZE);
        } while (false);
        return package;
    }

    /// \brief 将主体和操作转换为二进制数组直接发送
    /// \param[in] body 主体
    /// \param[in] operation 操作的编号
    /// \returns 包
    static inline LivePackage makeRequestPackage(const QByteArray &body, Operation operation)
    {
        LivePackage package;
        package.packageSize     = body.size() + HEADER_SIZE;
        package.headerSize      = HEADER_SIZE;
        package.protocolVersion = PROTOCOL_VERSION_REQUEST;
        package.operation       = operation;
        package.sequenceID      = SEQUENCE_ID;
        package.body            = body;
        return package;
    }

    /// \brief 制作进入房间的包
    /// \param[in] roomid 房间号
    /// \param[in] uid UID
    /// \returns 包
    static inline LivePackage makeEnterRoomPackage(int roomid, int uid = 0)
    {
        QJsonObject json;
        {
            using namespace Qt::Literals::StringLiterals;
            json["clientver"_L1] = "1.6.3"_L1; // 客户端版本 (非必须)
            json["platform"_L1]  = "web"_L1;   // 平台 (非必须)
            json["protover"_L1]  = 2;          // 协议版本 (1 或 2, 为 1 时不会使用 zlib 压缩, 否则可能使用, 但实际测试好像没用) (非必须)
            json["roomid"_L1]    = roomid;     // 房间号 (特殊房间需要用接口获取真实房间号) (必须)
            json["uid"_L1]       = uid;        // 进入房间的用户 (非必须)
            json["type"_L1]      = 2;          // 不知道是啥 (非必须)
        }
        return makeRequestPackage(QJsonDocument(json).toJson(QJsonDocument::Compact), ENTER_ROOM);
    }

    /// \brief 制作心跳包
    /// \returns 包
    static inline LivePackage makeHeartbeatPackage()
    {
        return makeRequestPackage(QByteArray(), HEARTBEAT);
    }
};

#endif
