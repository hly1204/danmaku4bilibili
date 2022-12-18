#include "gifttablemodel.h"
#include "danmakuclient.h"
#include <QAtomicInt>
#include <QDateTime>
#include <QDebug>
#include <QJsonValue>
#include <QModelIndex>
#include <QtGlobal>

using namespace Qt::Literals::StringLiterals;

GiftTableModel::GiftTableModel(QObject *parent)
    : QAbstractTableModel(parent),
      danmakuClient_(),
      header_({u"时间"_s, u"UID"_s, u"用户名"_s, u"礼物名"_s, u"价值(RMB 元)"_s}),
      totalGiftReceived_(0)
{
}

int GiftTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(gift_.size());
}

int GiftTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(header_.size());
}

QVariant GiftTableModel::data(const QModelIndex &index, int role) const
{
    do {
        if (!index.isValid()) break;
        const int r = index.row(), c = index.column();
        if (r >= rowCount(QModelIndex()) || c >= columnCount(QModelIndex())) break;

        switch (role) {
        case Qt::DisplayRole: {
            switch (c) {
            case 0: return gift_[r][c].toDateTime().toString("yyyy/MM/dd hh:mm:ss"_L1);
            default: return gift_[r][c];
            }
        }
        default: break;
        }
    } while (false);
    return {};
}

QVariant GiftTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (orientation) {
    case Qt::Horizontal:
        switch (role) {
        case Qt::DisplayRole: return header_[section];
        case Qt::SizeHintRole: return {};
        default: break;
        }
        [[fallthrough]];
    case Qt::Vertical: [[fallthrough]];
    default: return {};
    }
}

DanmakuClient *GiftTableModel::danmakuClient() const { return danmakuClient_; }

int GiftTableModel::totalGiftReceived() const { return totalGiftReceived_; }

void GiftTableModel::setDanmakuClient(DanmakuClient *client)
{
    if (danmakuClient_ != nullptr) {
        qWarning() << __func__ << "danmakuClient != nullptr"_L1;
        return;
    }
    danmakuClient_ = client;
    connect(danmakuClient_, &DanmakuClient::messageReceived, this, [this](const QJsonObject &json) {
        if (json["cmd"_L1].toString().startsWith("SEND_GIFT"_L1)) {
            const int r = rowCount(QModelIndex());
            beginInsertRows(QModelIndex(), r, r);
            const QJsonValue jsonData = json["data"_L1];
            // clang-format off
            int price = jsonData["price"_L1].toInt();
            gift_.emplaceBack() << QDateTime::currentDateTime()
                                << jsonData["uid"_L1].toInt()
                                << jsonData["uname"_L1].toString()
                                << jsonData["giftName"_L1].toString()
                                << price / 1000.;
            totalGiftReceived_ += price;
            // clang-format on
            endInsertRows();
        }
    });
}

void GiftTableModel::clear()
{
    beginResetModel();
    gift_.clear();
    endResetModel();
}
