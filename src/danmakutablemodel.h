#ifndef DANMAKU_TABLE_MODEL_H
#define DANMAKU_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QSize>
#include <QVariant>
#include <QVector>

class DanmakuClient;

class DanmakuTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(DanmakuTableModel)

public:
    explicit DanmakuTableModel(QObject *parent = nullptr);

    int      rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int      columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int roomid() const;

public slots:
    DanmakuClient *danmakuClient() const;
    void           setDanmakuClient(DanmakuClient *client);
    void           listen(int roomid);
    void           stop();
    void           clear();

private:
    DanmakuClient             *danmakuClient_ = {};
    QVector<QString>           header_        = {};
    QVector<QVector<QVariant>> danmaku_       = {};
    int                        roomid_        = {};
};

#endif
