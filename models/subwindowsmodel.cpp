#include "subwindowsmodel.h"
#include <QMdiSubWindow>
#include <QIcon>

SubWindowsModel::SubWindowsModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

SubWindowsModel::~SubWindowsModel()
{

}

QModelIndex SubWindowsModel::addWindow(QMdiSubWindow *wnd)
{
    if (m_Windows.contains(wnd))
        return index(m_Windows.indexOf(wnd), 0);

    QModelIndex nindex = index(m_Windows.size(), 0);
    beginInsertRows(QModelIndex(), m_Windows.size(), m_Windows.size());
    m_Windows.append(wnd);
    connect(wnd, &QMdiSubWindow::destroyed, this, &SubWindowsModel::windowClosed);
    endInsertRows();

    return nindex;
}

QMdiSubWindow *SubWindowsModel::window(const int &index)
{
    if (index >= 0 && index < m_Windows.size())
        return qobject_cast<QMdiSubWindow*>(m_Windows[index]);

    return nullptr;
}

QMdiSubWindow *SubWindowsModel::window(const QModelIndex &index)
{
    return window(index.row());
}

int SubWindowsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int SubWindowsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_Windows.size();
}

QVariant SubWindowsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_Windows.size())
        return QVariant();

    static QList<Qt::ItemDataRole> roles{Qt::DisplayRole, Qt::StatusTipRole, Qt::ToolTipRole};
    if (roles.contains(role))
        return m_Windows[index.row()]->windowTitle();
    else if (role == Qt::DecorationRole)
        return m_Windows[index.row()]->windowIcon();

    return QVariant();
}

void SubWindowsModel::windowClosed(QObject *wnd)
{
    QWidget *window = qobject_cast<QWidget*>(wnd);
    int pos = m_Windows.indexOf(window);

    if (pos != -1)
    {
        beginRemoveRows(QModelIndex(), pos, pos);
        m_Windows.removeAt(pos);
        endRemoveRows();
    }
}
