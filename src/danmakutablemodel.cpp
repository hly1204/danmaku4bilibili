#include "danmakutablemodel.h"
#include "danmakuclient.h"
#include <QDateTime>
#include <QDebug>
#include <QJsonValue>
#include <QtGlobal>

DanmakuTableModel::DanmakuTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    header_ << tr("Time") << tr("UID") << tr("Username") << tr("Danmaku");
}

int DanmakuTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(danmaku_.size());
}

int DanmakuTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(header_.size());
}

QVariant DanmakuTableModel::data(const QModelIndex &index, int role) const
{
    do {
        if (!index.isValid()) break;
        const int r = index.row(), c = index.column();
        if (r >= rowCount(QModelIndex()) || c >= columnCount(QModelIndex())) break;
        switch (role) {
        case Qt::DisplayRole: return danmaku_[r][c];
        default: break;
        }
    } while (false);
    return {};
}

QVariant DanmakuTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (orientation) {
    case Qt::Horizontal:
        switch (role) {
        case Qt::DisplayRole: return header_[section];
        default: break;
        }
        [[fallthrough]];
    case Qt::Vertical: [[fallthrough]];
    default: return {};
    }
}

DanmakuClient *DanmakuTableModel::danmakuClient() const { return danmakuClient_; }

void DanmakuTableModel::setDanmakuClient(DanmakuClient *client)
{
    if (danmakuClient_ != nullptr) {
        qWarning() << __func__ << "danmakuClient != nullptr";
        return;
    }
    connect(danmakuClient_ = client, &DanmakuClient::receivedDanmaku, this, [this](const QJsonValue &json) {
        /// TODO: 房间号判断过滤?
        const int r = rowCount(QModelIndex());
        beginInsertRows(QModelIndex(), r, r);
        const QJsonValue jsonInfo = json["info"];
        // clang-format off
            danmaku_.emplaceBack() << QDateTime::currentDateTime() << jsonInfo[2][0].toInt()
                                   << jsonInfo[2][1].toString()    << jsonInfo[1].toString();
        // clang-format on
        endInsertRows();
    });
}

void DanmakuTableModel::listen(int roomid)
{
    do {
        if (danmakuClient_ == nullptr) {
            qWarning() << __func__ << "danmakuClient == nullptr";
            break;
        }
        if (roomid_ != 0) {
            qWarning() << __func__ << "doubly called";
            break;
        }
        danmakuClient_->listen(roomid_ = roomid);
    } while (false);
}

void DanmakuTableModel::stop()
{
    if (danmakuClient_ != nullptr) {
        danmakuClient_->stop();
        roomid_ = 0;
    }
}

int DanmakuTableModel::roomid() const { return roomid_; }

void DanmakuTableModel::clear()
{
    beginResetModel();
    danmaku_.clear();
    endResetModel();
}
