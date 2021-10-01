#include "treeitem.h"
#include "treeitem.h"

TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent) :
    m_itemData(data),
    m_parentItem(parent)
{
    m_itemRoleData.resize(data.size());
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;

    return m_childItems.at(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::data(const int &column, const int &role) const
{
    if (column < 0 || column >= m_itemData.size())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return m_itemData.at(column);
    else
    {
        if (m_itemRoleData[column].contains(role))
            return m_itemRoleData[column][role];
    }

    return QVariant();
}

void TreeItem::setRoleData(const int &column, const int &role, const QVariant &value)
{
    if (column < 0 || column >= m_itemData.size())
        return;

    m_itemRoleData[column][role] = value;
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
