#include "columnalignedlayout.h"
#include <QHeaderView>

ColumnAlignedLayout::ColumnAlignedLayout(QWidget *parent) :
    QHBoxLayout(parent)
{

}

ColumnAlignedLayout::~ColumnAlignedLayout()
{

}

void ColumnAlignedLayout::setTableColumnsToTrack(QHeaderView *view)
{
    pHeaderView = view;
}

void ColumnAlignedLayout::setGeometry(const QRect &r)
{
    QHBoxLayout::setGeometry(r);

    Q_ASSERT_X(pHeaderView, "layout", "no table columns to track");
    if (!pHeaderView)
        return;

    if (pHeaderView->count() != count())
        return;

    Q_ASSERT(parentWidget());

    int widgetX = parentWidget()->mapToGlobal(QPoint(0, 0)).x();
    int headerX = pHeaderView->mapToGlobal(QPoint(0, 0)).x();
    int delta = headerX - widgetX;

    for (int ii = 0; ii < pHeaderView->count(); ++ii)
    {
        int pos = pHeaderView->sectionViewportPosition(ii);
        int size = pHeaderView->sectionSize(ii);

        auto item = itemAt(ii);
        auto r = item->geometry();
        r.setLeft(pos + delta);
        r.setWidth(size);
        item->setGeometry(r);
    }
}
