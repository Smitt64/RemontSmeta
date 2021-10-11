#ifndef COLUMNALIGNEDLAYOUT_H
#define COLUMNALIGNEDLAYOUT_H

#include <QHBoxLayout>

class QHeaderView;
class ColumnAlignedLayout : public QHBoxLayout
{
    Q_OBJECT
public:
    ColumnAlignedLayout(QWidget *parent = Q_NULLPTR);
    virtual ~ColumnAlignedLayout();

    void setTableColumnsToTrack(QHeaderView *view);

private:
    void setGeometry(const QRect &r) Q_DECL_OVERRIDE;
    QHeaderView *pHeaderView;
};

#endif // COLUMNALIGNEDLAYOUT_H
