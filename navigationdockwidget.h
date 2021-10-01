#ifndef NAVIGATIONDOCKWIDGET_H
#define NAVIGATIONDOCKWIDGET_H

#include <QDockWidget>
#include <QMainWindow>
#include <QObject>

class QTreeView;
class TreeModel;
class NavigationDockWidgetArea : public QMainWindow
{
    Q_OBJECT
public:
    NavigationDockWidgetArea(QWidget *parent = nullptr);
    virtual ~NavigationDockWidgetArea();

Q_SIGNALS:
    void itemClicked(const QString &mime);

private slots:
    void customContextMenuRequested(const QPoint &pos);
    void doubleClicked(const QModelIndex &index);

private:
    QTreeView *m_pView;
    TreeModel *m_pModel;
};

class NavigationDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    NavigationDockWidget(QWidget *parent = nullptr);
    virtual ~NavigationDockWidget();

Q_SIGNALS:
    void itemClicked(const QString &mime);

private:
    NavigationDockWidgetArea *m_pClientArea;
};

#endif // NAVIGATIONDOCKWIDGET_H
