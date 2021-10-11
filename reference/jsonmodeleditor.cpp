#include "jsonmodeleditor.h"
#include "models/jsontablemodel.h"
#include "widgets/filteredtablewidget.h"
#include <QTableView>
#include <QHeaderView>
#include <QToolBar>
#include <QMainWindow>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

class JsonTableModelFilterController : public FilteredControlHandler
{
public:
    JsonTableModelFilterController(JsonTableModel *model) :
        FilteredControlHandler(),
        pModel(model)
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

            w = spin;
        }
        else if (type == JsonTableModel::ColumnReal)
        {
            QDoubleSpinBox *spin = new QDoubleSpinBox();
            spin->setMinimum(minmax.width());
            spin->setMinimum(minmax.width());

            w = spin;
        }
        else
        {
            QLineEdit *edit = new QLineEdit();
            edit->setMaxLength(minmax.height());
            edit->setClearButtonEnabled(true);

            w = edit;
        }

        return w;
    }

private:
    JsonTableModel *pModel;
};

// ------------------------------------------------

JsonTableModelEditor::JsonTableModelEditor(QWidget *parent) :
    SubWindowBase(parent)
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
    m_NewRow = m_ToolBar->addAction(QIcon::fromTheme("NewRow"), "");
    m_RemoveRow = m_ToolBar->addAction(QIcon::fromTheme("RemoveRow"), "");
    m_ToolBar->addSeparator();
    m_GoToFirst = m_ToolBar->addAction(QIcon::fromTheme("GoToFirst"), "");
    m_GoToPrevious = m_ToolBar->addAction(QIcon::fromTheme("GoToPrevious"), "");
    m_GoToNext = m_ToolBar->addAction(QIcon::fromTheme("GoToNext"), "");
    m_GoToLast = m_ToolBar->addAction(QIcon::fromTheme("GoToLast"), "");

    setActionTextAndStatus(m_SaveTable, tr("Сохранить в файл"));
    setActionTextAndStatus(m_NewRow, tr("Новая запись"));
    setActionTextAndStatus(m_RemoveRow, tr("Удалить запись"));
    setActionTextAndStatus(m_GoToFirst, tr("К первой записи"));
    setActionTextAndStatus(m_GoToPrevious, tr("К предыдущей записи"));
    setActionTextAndStatus(m_GoToNext, tr("К следующей записи"));
    setActionTextAndStatus(m_GoToLast, tr("К последней записи"));

    setContentWidget(m_CentralWindow.data());
}

void JsonTableModelEditor::setJsonFile(const QString &filename)
{
    bool hr = m_Model->open(filename);

    if (!hr)
        ;
}

void JsonTableModelEditor::onInitModel()
{

}

void JsonTableModelEditor::showEvent(QShowEvent *event)
{
    if (!m_Model)
    {
        m_Model.reset(new JsonTableModel());
        onInitModel();
        m_FilterController.reset(new JsonTableModelFilterController(m_Model.data()));
        m_FilteredTable.reset(new FilteredTableWidget());
        m_FilteredTable->setController(m_FilterController.data());
        m_FilteredTable->setTableView(m_Table.data());
        m_CentralWindow->setCentralWidget(m_FilteredTable.data());
        m_Table->setModel(m_Model.data());

        for (int i = 0; i < m_Model->columnCount(); i++)
        {
            const JsonTableModel::ColumnWidthType &w = m_Model->columnWidth(i);

            if (w)
                m_Table->setColumnWidth(i, *w);
        }
    }

    SubWindowBase::showEvent(event);
}
