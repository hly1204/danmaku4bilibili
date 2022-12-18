#ifndef GIFT_TABLE_MODEL_H
#define GIFT_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QAtomicInt>
#include <QSize>
#include <QString>
#include <QVariant>

// CUSTOM
class DanmakuClient;

class GiftTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int totalGiftReceived READ totalGiftReceived)

public:
    explicit GiftTableModel(QObject *parent = nullptr);

    int      rowCount(const QModelIndex &parent) const override;
    int      columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    DanmakuClient *danmakuClient() const;
    int            totalGiftReceived() const;

public slots:
    void setDanmakuClient(DanmakuClient *client);
    void clear();

private:
    Q_DISABLE_COPY_MOVE(GiftTableModel)

private:
    DanmakuClient             *danmakuClient_;
    QVector<QString>           header_;
    QVector<QVector<QVariant>> gift_;
    QAtomicInt                 totalGiftReceived_;
};

#endif
