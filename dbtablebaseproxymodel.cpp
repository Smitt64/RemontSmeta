#include "dbtablebaseproxymodel.h"
#include "db/dbtable.h"

DbTableBaseProxyModel::DbTableBaseProxyModel(DbTable *table, QObject *parent) :
    QAbstractProxyModel(parent),
    m_Table(table)
{

}

DbTableBaseProxyModel::~DbTableBaseProxyModel()
{

}

QModelIndex DbTableBaseProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return sourceIndex;
}

QModelIndex DbTableBaseProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return proxyIndex;
}

QModelIndex DbTableBaseProxyModel::index(int row, int col, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, col);
}

QModelIndex DbTableBaseProxyModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int DbTableBaseProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Table->rowCount();
}

int DbTableBaseProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Table->columnCount();
}

DbTable *DbTableBaseProxyModel::table() const
{
    return m_Table;
}

Qt::ItemFlags DbTableBaseProxyModel::flags(const QModelIndex &index) const
{
    return m_Table->flags(index);
}

void DbTableBaseProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, &QAbstractProxyModel::rowsInserted, this, &DbTableBaseProxyModel::rowsInserted);
    connect(sourceModel, &QAbstractProxyModel::modelReset, this, &DbTableBaseProxyModel::modelReset);
}
