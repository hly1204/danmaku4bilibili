#ifndef DANMAKU_TABLE_MODEL_H_
#define DANMAKU_TABLE_MODEL_H_

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

public:
    explicit DanmakuTableModel(QObject *parent = nullptr);

    int      rowCount(const QModelIndex &parent) const override;
    int      columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    DanmakuClient *danmakuClient() const;

public slots:
    void setDanmakuClient(DanmakuClient *client);
    void clear();

private:
    Q_DISABLE_COPY_MOVE(DanmakuTableModel)

private:
    DanmakuClient             *danmakuClient_;
    QVector<QString>           header_;
    QVector<QVector<QVariant>> danmaku_;
};

#endif
