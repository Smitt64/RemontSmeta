#ifndef SUBWINDOWBASE_H
#define SUBWINDOWBASE_H

#include <QMdiSubWindow>

class SubWindowBase : public QMdiSubWindow
{
    Q_OBJECT
public:
    SubWindowBase(QWidget *parent = nullptr);
    virtual ~SubWindowBase();

    void setWidget(QWidget *widget) = delete;
    void setContentWidget(QWidget *widget);

protected:
    void setActionTextAndStatus(QAction *action, const QString &str);

signals:
    void requestAddSubWindow(QWidget *widget);
};

#endif // SUBWINDOWBASE_H
