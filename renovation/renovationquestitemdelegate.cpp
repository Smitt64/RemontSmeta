#include "renovationquestitemdelegate.h"
#include "renovation/renovationquesttionmodel.h"
#include "globals.h"
#include "models/jsontablemodel.h"
#include <QComboBox>
#include <QStyleOptionViewItem>
#include <QPalette>
#include <QPainter>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QStyle>
#include <QApplication>
#include <QStyleOptionHeader>
#include <QCheckBox>

RenovationQuestItemDelegate::RenovationQuestItemDelegate(RenovationQuestionModel *model, QObject *parent) :
    QStyledItemDelegate(parent),
    m_pModel(model)
{

}

RenovationQuestItemDelegate::~RenovationQuestItemDelegate()
{
}

QWidget *RenovationQuestItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *widget = nullptr;

    RenovationQuestionModelElement *element = m_pModel->element(index.row());
    if (element->type() == RenovationQuestionModelElement::TypeComboBox)
    {
        QComboBox *combo = new QComboBox(parent);
        combo->setModel(element->comboModel());
        combo->setModelColumn(1);

        return combo;
    }
    else if (element->type() == RenovationQuestionModelElement::TypeModel)
    {
        JsonTableModel *model = Globals::inst()->model(element->jsonModelName());
        int columnNumber = model->columnNumber(element->jsonModelDisplay());

        QComboBox *combo = new QComboBox(parent);
        combo->setModel(model);
        combo->setModelColumn(columnNumber);

        return combo;
    }
    else if (element->type() == RenovationQuestionModelElement::TypeValue)
    {
        DbFieldBase *fld = m_pModel->field(index);

        if (fld->type() == DbFieldBase::Boolean)
        {
            QComboBox *combo = new QComboBox(parent);
            combo->addItems({tr("нет"), tr("да")});
            return combo;
        }
        else
            widget = QStyledItemDelegate::createEditor(parent, option, index);
    }
    else
        widget = QStyledItemDelegate::createEditor(parent, option, index);

    return widget;
}

void RenovationQuestItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    RenovationQuestionModelElement *element = m_pModel->element(index.row());

    if (element->type() == RenovationQuestionModelElement::TypeComboBox)
    {
        QComboBox *combo = qobject_cast<QComboBox*>(editor);

        if (combo)
        {
            QVariant val = index.data(Qt::EditRole);
            combo->setCurrentIndex(val.toInt());
        }
    }
    else if (element->type() == RenovationQuestionModelElement::TypeModel)
    {
        QComboBox *combo = qobject_cast<QComboBox*>(editor);
        JsonTableModel *jsmodel = Globals::inst()->model(element->jsonModelName());

        if (combo)
        {
            QVariant val = index.data(Qt::EditRole);
            QModelIndex idx = jsmodel->getValueIndex(element->jsonModelKey(), val);
            combo->setCurrentIndex(idx.row());
        }
    }
    else
    {
        DbFieldBase *fld = m_pModel->field(index);
        if (fld->type() == DbFieldBase::Boolean)
        {
            QVariant val = index.data(Qt::EditRole);
            QComboBox *combo = qobject_cast<QComboBox*>(editor);

            if (val.canConvert<int>() && combo)
                combo->setCurrentIndex(val.value<int>());
        }
        else
            QStyledItemDelegate::setEditorData(editor, index);
    }
}

void RenovationQuestItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    RenovationQuestionModelElement *element = m_pModel->element(index.row());

    if (element->type() == RenovationQuestionModelElement::TypeComboBox)
    {
        QComboBox *combo = qobject_cast<QComboBox*>(editor);

        if (combo)
        {
            QAbstractItemModel *comboModel = combo->model();
            QModelIndex idx = comboModel->index(combo->currentIndex(), 0);
            QVariant val = comboModel->data(idx);
            model->setData(index, val);
        }
    }
    else if (element->type() == RenovationQuestionModelElement::TypeModel)
    {
        QComboBox *combo = qobject_cast<QComboBox*>(editor);
        JsonTableModel *jsmodel = Globals::inst()->model(element->jsonModelName());

        int columnNumber = jsmodel->columnNumber(element->jsonModelKey());
        QVariant id = jsmodel->data(jsmodel->index(combo->currentIndex(), columnNumber));
        model->setData(index, id);
    }
    else
    {
        DbFieldBase *fld = m_pModel->field(index);
        if (fld->type() == DbFieldBase::Boolean)
        {
            QComboBox *combo = qobject_cast<QComboBox*>(editor);
            model->setData(index, combo->currentIndex());
        }
        else
            QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void RenovationQuestItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    RenovationQuestionModelElement *element = m_pModel->element(index.row());

    if (element->isGroup())
    {
        QStyle *style = qApp->style();

        QStyleOptionHeader header;
        header.initFrom(opt.widget);
        header.rect = opt.rect;
        header.state = opt.state;
        header.textAlignment = Qt::AlignVCenter | Qt::AlignLeft;

        if (index.column() == RenovationQuestionModel::FldName)
            header.text = element->title();

        style->drawControl(QStyle::CE_Header, &header, painter);
    }
    else
    {
        /*if (element->isEditable())
        {
            if (index.row() % 2 != 0)
            {
                QColor color = option.palette.color(QPalette::ToolTipBase);
                opt.backgroundBrush = QBrush(color);
                painter->fillRect(option.rect, opt.backgroundBrush);
            }
        }
        else
        {
            QColor color = option.palette.color(QPalette::Window);
            opt.backgroundBrush = QBrush(color);
            painter->fillRect(option.rect, opt.backgroundBrush);

            //opt.state &= ~QStyle::State_Enabled;
        }*/

        QStyledItemDelegate::paint(painter, opt, index);
    }
}
