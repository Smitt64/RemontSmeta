#ifndef SUBWINDOWSMODEL_H
#define SUBWINDOWSMODEL_H

#include <QAbstractListModel>

class QMdiSubWindow;
class SubWindowsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SubWindowsModel(QObject *parent = nullptr);
    virtual ~SubWindowsModel();

    QModelIndex addWindow(QMdiSubWindow *wnd);
    QMdiSubWindow *window(const QModelIndex &index);

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private slots:
    void windowClosed(QObject*);

private:
    QList<QWidget*> m_Windows;
};

#endif // SUBWINDOWSMODEL_H
