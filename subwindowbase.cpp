#include "subwindowbase.h"
#include <QDebug>
#include <QCloseEvent>
#include <QIcon>
#include <QAction>

SubWindowBase::SubWindowBase(QWidget *parent) :
    QMdiSubWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

SubWindowBase::~SubWindowBase()
{

}

void SubWindowBase::setContentWidget(QWidget *widget)
{
    QMdiSubWindow::setWidget(widget);
    setWindowIcon(widget->windowIcon());

    connect(widget, &QWidget::windowTitleChanged, this, &SubWindowBase::setWindowTitle);
}

void SubWindowBase::setActionTextAndStatus(QAction *action, const QString &str)
{
    action->setText(str);
    action->setStatusTip(str);
}

void SubWindowBase::setParam(const QString &param)
{
    m_Param = param;
}

const QString &SubWindowBase::param() const
{
    return m_Param;
}
