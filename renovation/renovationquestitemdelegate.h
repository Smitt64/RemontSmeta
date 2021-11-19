#ifndef RENOVATIONQUESTITEMDELEGATE_H
#define RENOVATIONQUESTITEMDELEGATE_H

#include <QStyledItemDelegate>

class RenovationQuestionModel;
class RenovationQuestItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    RenovationQuestItemDelegate(RenovationQuestionModel *model, QObject *parent = nullptr);
    virtual ~RenovationQuestItemDelegate();

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    RenovationQuestionModel *m_pModel;
};

#endif // RENOVATIONQUESTITEMDELEGATE_H
