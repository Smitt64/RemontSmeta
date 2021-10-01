#ifndef DBTABLEBASEPROXYMODEL_H
#define DBTABLEBASEPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QObject>

class DbTable;
class DbTableBaseProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    DbTableBaseProxyModel(DbTable *table, QObject *parent = nullptr);
    virtual ~DbTableBaseProxyModel();

    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int col, const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual void setSourceModel(QAbstractItemModel *sourceModel) Q_DECL_OVERRIDE;

    DbTable *table() const;

private:
    DbTable *m_Table;
};

#endif // DBTABLEBASEPROXYMODEL_H
