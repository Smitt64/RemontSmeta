#ifndef FILTEREDTABLEWIDGET_H
#define FILTEREDTABLEWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>

class FilteredControlHandler
{
public:
    virtual QWidget *create(const int &index) = 0;
    virtual ~FilteredControlHandler() { }
};

class ColumnAlignedLayout;
class FilteredTableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilteredTableWidget(QWidget *parent = nullptr);
    void setTableView(QTableView *table);

    void setController(FilteredControlHandler *controller);

private slots:
    void invalidateAlignedLayout();
    void onSectionCountChanged(int oldCount, int newCount);

private:
    ColumnAlignedLayout *pTableFitersLayout;
    QTableView *pTable;
    QVBoxLayout *pLayout;
    FilteredControlHandler *pCotroller;
    bool m_recreateAll;
};

#endif // FILTEREDTABLEWIDGET_H
