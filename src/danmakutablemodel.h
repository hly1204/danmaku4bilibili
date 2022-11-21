#ifndef DANMAKU_TABLE_MODEL_H
#define DANMAKU_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QSize>
#include <QString>
#include <QVariant>
#include <QVector>

// CUSTOM
class DanmakuClient;

class DanmakuTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(DanmakuTableModel)

public:
    explicit DanmakuTableModel(QObject *parent = nullptr);

    int      rowCount(const QModelIndex &parent) const override;
    int      columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    int            roomid() const;
    DanmakuClient *danmakuClient() const;

public slots:
    void setDanmakuClient(DanmakuClient *client);
    void listen(int roomid);
    void stop();
    void clear();

private:
    DanmakuClient             *danmakuClient_;
    QVector<QString>           header_;
    QVector<QSize>             headerSize_;
    QVector<QVector<QVariant>> danmaku_;
    int                        roomid_;
};

#endif
