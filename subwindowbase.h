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

    virtual void setParam(const QString &param);

    const QString &param() const;

protected:
    void setActionTextAndStatus(QAction *action, const QString &str);

signals:
    void requestAddSubWindow(QWidget *widget);

private:
    QString m_Param;
};

#endif // SUBWINDOWBASE_H
