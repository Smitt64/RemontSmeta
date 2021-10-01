#ifndef SUBWINDOWBASE_H
#define SUBWINDOWBASE_H

#include <QMdiSubWindow>

class SubWindowBase : public QMdiSubWindow
{
    Q_OBJECT
public:
    SubWindowBase();
    virtual ~SubWindowBase();
};

#endif // SUBWINDOWBASE_H
