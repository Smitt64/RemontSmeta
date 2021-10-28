#include "jsonmodeleditor.h"
#include "models/jsontablemodel.h"
#include "widgets/filteredtablewidget.h"
#include "globals.h"
#include <QTableView>
#include <QHeaderView>
#include <QToolBar>
#include <QMainWindow>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QMapIterator>
#include <QItemSelectionModel>
#include <QFileInfo>
#include <QMessageBox>

JsonTableModelFilter::JsonTableModelFilter(QObject *parent) :
    QSortFilterProxyModel(parent)
{

}

JsonTableModelFilter::~JsonTableModelFilter()
{

}

void JsonTableModelFilter::setValue(const quint16 &column, const QVariant &value)
{
    m_Filters[column] = value;
    invalidateFilter();

    emit filterChanged();
}

void JsonTableModelFilter::clearFilter()
{
    m_Filters.clear();
    invalidateFilter();

    emit filterChanged();
}

bool JsonTableModelFilter::isFiltered() const
{
    return !m_Filters.isEmpty();
}

bool JsonTableModelFilter::isLastIndex(const QModelIndex &index) const
{
    bool hr = false;

    if (index.row() == rowCount() - 1)
        hr = true;

    return hr;
}

bool JsonTableModelFilter::isFirstIndex(const QModelIndex &index) const
{
    bool hr = false;

    if (rowCount() && index.row() == 0)
        hr = true;

    return hr;
}

bool JsonTableModelFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);
    if (m_Filters.isEmpty())
        return true;

    bool result = false;
    quint16 trueCount = 0;
    JsonTableModel *srcModel = qobject_cast<JsonTableModel*>(sourceModel());

    QMapIterator iter(m_Filters);
    while(iter.hasNext())
    {
        const auto &value = iter.next();
        const auto &key = value.key();
        const auto &keyValue = value.value();
        const JsonTableModel::ColumnType &type = srcModel->columnType(key);

        QVariant modeldata = srcModel->data(srcModel->index(source_row, key));
        if (type == JsonTableModel::ColumnString)
        {
            if (modeldata.toString().contains(keyValue.toString(), Qt::CaseInsensitive))
                trueCount ++;
        }
        else if (type == JsonTableModel::ColumnAutoInc || type == JsonTableModel::ColumnInteger)
        {
            qint32 modelValue = modeldata.value<qint32>();
            qint32 filterValue = keyValue.value<qint32>();

            if (modelValue == filterValue)
                trueCount++;
        }
        else if (type == JsonTableModel::ColumnReal)
        {
            qreal modelValue = modeldata.value<qreal>();
            qreal filterValue = keyValue.value<qreal>();

            if (modelValue == filterValue)
                trueCount++;
        }
    }

    if (trueCount == m_Filters.size())
        result = true;

    return result;
}

// ------------------------------------------------

class JsonTableModelFilterController : public FilteredControlHandler
{
public:
    JsonTableModelFilterController(JsonTableModel *model, JsonTableModelFilter *filter) :
        FilteredControlHandler(),
        pModel(model),
        pFilter(filter)
    {

    }

    virtual ~JsonTableModelFilterController() { }

    virtual QWidget *create(const int &index) Q_DECL_OVERRIDE
    {
        QWidget *w = nullptr;
        JsonTableModel::ColumnType type = pModel->columnType(index);
        QSizeF minmax = pModel->columnMinMax(index);

        if (type == JsonTableModel::ColumnAutoInc || type == JsonTableModel::ColumnInteger)
        {
            QSpinBox *spin = new QSpinBox();
            spin->setMinimum(minmax.width());
            spin->setMinimum(minmax.width());
            spin->setAlignment(Qt::AlignRight);

            QObject::connect(spin, &QSpinBox::valueChanged, [=](const int &val) -> void
            {
                pFilter->setValue(index, val);
            });

            w = spin;
        }
        else if (type == JsonTableModel::ColumnReal)
        {
            QDoubleSpinBox *spin = new QDoubleSpinBox();
            spin->setMinimum(minmax.width());
            spin->setMinimum(minmax.width());
            spin->setAlignment(Qt::AlignRight);

            QObject::connect(spin, &QDoubleSpinBox::valueChanged, [=](const double &val) -> void
            {
                pFilter->setValue(index, val);
            });

            w = spin;
        }
        else
        {
            QLineEdit *edit = new QLineEdit();
            edit->setMaxLength(minmax.height());
            edit->setClearButtonEnabled(true);

            QObject::connect(edit, &QLineEdit::textChanged, [=](const QString &val) -> void
            {
                pFilter->setValue(index, val);
            });

            w = edit;
        }

        m_Widgets[index] = w;

        return w;
    }

    void clearValue(const int &index)
    {
        JsonTableModel::ColumnType type = pModel->columnType(index);

        if (type == JsonTableModel::ColumnAutoInc || type == JsonTableModel::ColumnInteger)
        {
            QSpinBox *spin = qobject_cast<QSpinBox*>(m_Widgets[index]);

            if (spin)
                spin->setValue(0);
        }
        else if (type == JsonTableModel::ColumnReal)
        {
            QDoubleSpinBox *spin = qobject_cast<QDoubleSpinBox*>(m_Widgets[index]);

            if (spin)
                spin->setValue(0.0);
        }
        else
        {
            QLineEdit *edit = qobject_cast<QLineEdit*>(m_Widgets[index]);

            if (edit)
                edit->setText(QString());
        }
    }

    void clearValues()
    {
        QMapIterator iter(m_Widgets);

        while (iter.hasNext())
        {
            const auto &element = iter.next();
            clearValue(element.key());
        }
    }

private:
    QMap<quint16, QWidget*> m_Widgets;
    JsonTableModel *pModel;
    JsonTableModelFilter *pFilter;
};

// ------------------------------------------------

JsonTableModelEditor::JsonTableModelEditor(QWidget *parent) :
    SubWindowBase(parent),
    m_IsFirstShow(true)
{
    setupWindow();
}

JsonTableModelEditor::~JsonTableModelEditor()
{

}

void JsonTableModelEditor::setupWindow()
{
    m_CentralWindow.reset(new QMainWindow());
    m_Table.reset(new QTableView());
    m_Table->verticalHeader()->setDefaultSectionSize(20);
    m_ToolBar.reset(new QToolBar());

    m_CentralWindow->addToolBar(Qt::TopToolBarArea, m_ToolBar.data());

    m_ToolBar->setIconSize(QSize(16,16));
    m_SaveTable = m_ToolBar->addAction(QIcon::fromTheme("SaveTable"), "");
    m_ToolBar->addSeparator();
    m_ApplyChanges = m_ToolBar->addAction(QIcon::fromTheme("ApplyTable"), "");
    m_RevertChanges = m_ToolBar->addAction(QIcon::fromTheme("RevertTable"), "");
    m_ToolBar->addSeparator();
    m_FilterAction = m_ToolBar->addAction(QIcon::fromTheme("ClearFilter"), "");
    m_ToolBar->addSeparator();
    m_NewRow = m_ToolBar->addAction(QIcon::fromTheme("AddRow"), "");
    m_RemoveRow = m_ToolBar->addAction(QIcon::fromTheme("RemoveRow"), "");
    m_ToolBar->addSeparator();
    m_GoToFirst = m_ToolBar->addAction(QIcon::fromTheme("GoToFirst"), "");
    m_GoToPrevious = m_ToolBar->addAction(QIcon::fromTheme("GoToPrevious"), "");
    m_GoToNext = m_ToolBar->addAction(QIcon::fromTheme("GoToNext"), "");
    m_GoToLast = m_ToolBar->addAction(QIcon::fromTheme("GoToLast"), "");

    setActionTextAndStatus(m_SaveTable, tr("Сохранить в файл"));
    setActionTextAndStatus(m_ApplyChanges, tr("Применить изменения"));
    setActionTextAndStatus(m_RevertChanges, tr("Отменить изменения"));
    setActionTextAndStatus(m_NewRow, tr("Новая запись"));
    setActionTextAndStatus(m_RemoveRow, tr("Удалить запись"));
    setActionTextAndStatus(m_GoToFirst, tr("К первой записи"));
    setActionTextAndStatus(m_GoToPrevious, tr("К предыдущей записи"));
    setActionTextAndStatus(m_GoToNext, tr("К следующей записи"));
    setActionTextAndStatus(m_GoToLast, tr("К последней записи"));
    setActionTextAndStatus(m_FilterAction, tr("Сбросить фильтр"));

    setContentWidget(m_CentralWindow.data());
}

bool JsonTableModelEditor::setJsonFile(const QString &filename)
{
    bool hr = true;
    QFileInfo fi(filename);

    if (Globals::inst()->hasJsonModel(fi.baseName()))
    {
        JsonTableModel *model = Globals::inst()->model(fi.baseName());
        m_Model.set(model);
    }
    else
    {
        m_Model.create(new JsonTableModel());
        hr = m_Model->open(filename);
    }

    return hr;
}

bool JsonTableModelEditor::onInitModel()
{
    return false;
}

void JsonTableModelEditor::showEvent(QShowEvent *event)
{
    if (m_IsFirstShow)
    {
        bool inited = false;
        if (!(inited = onInitModel()))
            inited = setJsonFile(param());

        if (!inited)
        {
            setWindowTitle(tr("Спарвочник: %1").arg(param()));
            setWindowIcon(QIcon::fromTheme("TableError"));
            QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл справочника <b>%1</b>").arg(param()));

            m_Model.destroy();
        }
        else
        {
            QString title;
            QString metatitle = m_Model->metaTitle();

            if (!metatitle.isEmpty())
                title = metatitle;
            else
            {
                QFileInfo fi(param());
                title = fi.fileName();
            }

            setWindowTitle(tr("Спарвочник: %1").arg(title));

            m_Filter.reset(new JsonTableModelFilter());
            m_FilterController.reset(new JsonTableModelFilterController(m_Model.data(), m_Filter.data()));
            m_FilteredTable.reset(new FilteredTableWidget());
            m_FilteredTable->setController(m_FilterController.data());
            m_FilteredTable->setTableView(m_Table.data());
            m_CentralWindow->setCentralWidget(m_FilteredTable.data());

            m_Filter->setSourceModel(m_Model.data());
            m_Table->setModel(m_Filter.data());

            for (int i = 0; i < m_Model->columnCount(); i++)
            {
                const JsonTableModel::ColumnWidthType &w = m_Model->columnWidth(i);

                if (w)
                    m_Table->setColumnWidth(i, *w);
            }

            connect(m_FilterAction, &QAction::triggered, [=]()
            {
                m_FilterController->clearValues();
                m_Filter->clearFilter();
                updateActions();
            });

            connect(m_Filter.data(), &JsonTableModelFilter::filterChanged, this, &JsonTableModelEditor::updateActions);
            connect(m_Table->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &JsonTableModelEditor::updateActions);
            connect(m_Model.data(), &JsonTableModel::editStateChanged, this, &JsonTableModelEditor::updateActions);

            connect(m_NewRow, &QAction::triggered, this, &JsonTableModelEditor::insertRow);
            connect(m_RevertChanges, &QAction::triggered, m_Model.data(), &JsonTableModel::revertChanges);

            connect(m_SaveTable, &QAction::triggered, m_Model.data(), &JsonTableModel::save);
        }

        updateActions();
        m_IsFirstShow = false;
    }

    SubWindowBase::showEvent(event);
}

void JsonTableModelEditor::updateActions()
{
    if (!m_Model)
    {
        m_FilterAction->setEnabled(false);
        m_ApplyChanges->setEnabled(false);
        m_RevertChanges->setEnabled(false);
        m_GoToPrevious->setEnabled(false);
        m_GoToNext->setEnabled(false);
        m_RemoveRow->setEnabled(false);
        m_GoToFirst->setEnabled(false);
        m_GoToLast->setEnabled(false);
        m_NewRow->setEnabled(false);
        m_SaveTable->setEnabled(false);
        return;
    }

    if (!m_Filter)
        m_FilterAction->setEnabled(false);
    else
        m_FilterAction->setEnabled(m_Filter->isFiltered());

    bool hasChanges = m_Model->hasChanges();
    m_ApplyChanges->setEnabled(hasChanges);
    m_RevertChanges->setEnabled(hasChanges);

    QModelIndex index = m_Table->currentIndex();
    if (!index.isValid())
    {
        m_GoToPrevious->setEnabled(false);
        m_GoToNext->setEnabled(false);
        m_RemoveRow->setEnabled(false);
        m_GoToFirst->setEnabled(false);
        m_GoToLast->setEnabled(false);
    }
    else
    {
        m_RemoveRow->setEnabled(true);

        m_GoToNext->setEnabled(!m_Filter->isLastIndex(index));
        m_GoToLast->setEnabled(!m_Filter->isLastIndex(index));

        m_GoToFirst->setEnabled(!m_Filter->isFirstIndex(index));
        m_GoToPrevious->setEnabled(!m_Filter->isFirstIndex(index));
    }
}

void JsonTableModelEditor::insertRow()
{
    m_Model->insertRow(m_Model->rowCount());
    updateActions();
}
