#include "filteredtablewidget.h"

#include "columnalignedlayout.h"
#include <QComboBox>
#include <QHeaderView>
#include <QScrollBar>

FilteredTableWidget::FilteredTableWidget(QWidget *parent) :
    QWidget(parent),
    pTable(Q_NULLPTR),
    pCotroller(Q_NULLPTR),
    m_recreateAll(false)
{
    pLayout = new QVBoxLayout();
    pTableFitersLayout = new ColumnAlignedLayout();
    pTableFitersLayout->setSpacing(2);
    setLayout(pLayout);
    //pTableFitersLayout->setParent(this);
    //pLayout->setMargin(0);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addLayout(pTableFitersLayout);
    pTableFitersLayout->setContentsMargins(0, 0, 0, 0);
}

void FilteredTableWidget::setTableView(QTableView *table)
{
    pTable = table;
    pTableFitersLayout->setTableColumnsToTrack(pTable->horizontalHeader());
    pLayout->addWidget(table);

    connect(pTable->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), SLOT(invalidateAlignedLayout()));
    connect(pTable->horizontalScrollBar(), SIGNAL(valueChanged(int)), SLOT(invalidateAlignedLayout()));
    connect(pTable->horizontalHeader(), SIGNAL(sectionCountChanged(int,int)), SLOT(onSectionCountChanged(int,int)));
}

void FilteredTableWidget::invalidateAlignedLayout()
{
    pTableFitersLayout->invalidate();
}

void FilteredTableWidget::setController(FilteredControlHandler *controller)
{
    pCotroller = controller;
}

void FilteredTableWidget::onSectionCountChanged(int oldCount, int newCount)
{
    Q_UNUSED(oldCount);

    if (!m_recreateAll)
    {
        while (pTableFitersLayout->count() > newCount)
        {
            QWidget *w = pTableFitersLayout->itemAt(pTableFitersLayout->count() - 1)->widget();
            pTableFitersLayout->removeWidget(w);
            delete w;
        }

        while (pTableFitersLayout->count() < newCount)
        {
            if (pCotroller)
            {
                QWidget *w = pCotroller->create(pTableFitersLayout->count());
                pTableFitersLayout->addWidget(w);
            }
            else {
                break;
            }
        }
    }
    else
    {
        while (pTableFitersLayout->count())
        {
            QWidget *w = pTableFitersLayout->itemAt(pTableFitersLayout->count() - 1)->widget();
            pTableFitersLayout->removeWidget(w);
            delete w;
        }

        for (int i = 0; i < newCount; i++)
        {
            if (pCotroller)
            {
                QWidget *w = pCotroller->create(i);
                pTableFitersLayout->addWidget(w);
            }
        }
    }
}
