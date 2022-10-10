#include "danmakutablemodel.h"
#include "danmakuclient.h"
#include <QDateTime>
#include <QDebug>
#include <QJsonValue>
#include <QtGlobal>

DanmakuTableModel::DanmakuTableModel(QObject *parent)
    : QAbstractTableModel(parent), danmakuClient_(new DanmakuClient(this)), header_(), danmaku_(), roomid_(0) {
  header_ << tr("Time") << tr("UID") << tr("Username") << tr("Danmaku");
  connect(danmakuClient_, &DanmakuClient::receivedDanmaku, this, [this](const QJsonValue &json) {
    /// TODO: 房间号判断过滤?
    int r = rowCount();
    beginInsertRows(QModelIndex(), r, r);
    const QJsonValue jsonInfo = json["info"];
    danmaku_.emplaceBack() << QDateTime::currentDateTime() << jsonInfo[2][0].toInt() << jsonInfo[2][1].toString()
                           << jsonInfo[1].toString();
    endInsertRows();
  });
}

int DanmakuTableModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return danmaku_.size();
}

int DanmakuTableModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return header_.size();
}

QVariant DanmakuTableModel::data(const QModelIndex &index, int role) const {
  do {
    if (!index.isValid())
      break;
    int r = index.row(), c = index.column();
    if (r >= rowCount() || c >= columnCount())
      break;
    switch (role) {
    case Qt::DisplayRole:
      return danmaku_[r][c];
    default:
      break;
    }
  } while (false);
  return QVariant();
}

QVariant DanmakuTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  switch (orientation) {
  case Qt::Horizontal:
    switch (role) {
    case Qt::DisplayRole:
      return header_[section];
    default:
      break;
    }
    [[fallthrough]];
  case Qt::Vertical:
    [[fallthrough]];
  default:
    return QVariant();
  }
}

void DanmakuTableModel::listen(int roomid) {
  if (roomid_ == 0) {
    danmakuClient_->listen(roomid_ = roomid);
  } else {
    qWarning() << __func__ << "doubly called";
  }
}

int DanmakuTableModel::roomid() const { return roomid_; }

void DanmakuTableModel::clear() {
  beginResetModel();
  danmaku_.clear();
  endResetModel();
}
