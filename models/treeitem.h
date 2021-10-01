#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QVector>

class TreeItem
{
public:
    explicit TreeItem(const QVector<QVariant> &data, TreeItem *parentItem = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(const int &column, const int &role = Qt::DisplayRole) const;
    int row() const;
    TreeItem *parentItem();

    void setRoleData(const int &column, const int &role, const QVariant &value);

private:
    typedef QMap<int, QVariant> RoleDataMap;
    QVector<TreeItem*> m_childItems;
    QVector<QVariant> m_itemData;
    QVector<RoleDataMap> m_itemRoleData;
    TreeItem *m_parentItem;
};

#endif // TREEITEM_H
