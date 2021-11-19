#include "selectpartydlg.h"
#include "ui_selectpartydlg.h"
#include "clientswindow/clientsviewmodel.h"
#include "tables/tclient.h"
#include "person/personpanel.h"
#include <QMainWindow>
#include <QListView>
#include <QToolBar>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QPushButton>
#include <QMenu>

class ClientListModel : public QSortFilterProxyModel
{
public:
    ClientListModel(QObject *parent = nullptr) :
        QSortFilterProxyModel(parent)
    {

    }

    virtual ~ClientListModel()
    {

    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
    {
        if (role == Qt::DecorationRole)
        {
            QAbstractItemModel *sourceModel = this->sourceModel();
            QModelIndex sourceindex = mapToSource(index);
            return sourceModel->data(this->index(sourceindex.row(), ClientsViewModel::FldSex), Qt::DecorationRole).value<QIcon>();
        }

        return QSortFilterProxyModel::data(index, role);
    }
};

SelectPartyDlg::SelectPartyDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectPartyDlg)
{
    ui->setupUi(this);
    m_pWindow.reset(new QMainWindow());
    m_ClientsView.reset(new QListView());
    m_MainToolBar.reset(new QToolBar());
    m_FilterEdit.reset(new QLineEdit());
    m_ClientsModel.reset(new ClientsViewModel());
    m_FilterModel.reset(new ClientListModel());

    m_MainToolBar->setMovable(false);
    m_MainToolBar->setFloatable(false);
    m_MainToolBar->setIconSize(QSize(16,16));

    m_ClientsModel->setPartyOwns({TPersonOwn::OwnClient});
    m_FilterModel->setSourceModel(m_ClientsModel.data());
    m_FilterModel->setFilterKeyColumn(ClientsViewModel::FldFullName);
    m_FilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_ClientsModel->resetQuery();

    m_FilterEdit->setClearButtonEnabled(true);
    m_FilterEdit->setPlaceholderText(tr("Поиск субъекта"));

    m_pNewParty = m_MainToolBar->addAction(QIcon::fromTheme("NewTeam"), tr("Новый субъект"));
    m_pViewParty = m_MainToolBar->addAction(QIcon::fromTheme("ViewTeam"), tr("Просмотр субъекта"));
    m_MainToolBar->addSeparator();
    m_MainToolBar->addWidget(m_FilterEdit.data());
    m_pViewParty->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    m_ClientsView->setModel(m_FilterModel.data());
    m_ClientsView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_ClientsView->setModelColumn(ClientsViewModel::FldFullName);
    m_pWindow->setCentralWidget(m_ClientsView.data());
    m_pWindow->addToolBar(Qt::TopToolBarArea, m_MainToolBar.data());
    ui->verticalLayout->insertWidget(0, m_pWindow.data());

    connect(m_FilterEdit.data(), &QLineEdit::textChanged, m_FilterModel.data(), &QSortFilterProxyModel::setFilterFixedString);
    connect(m_pViewParty, &QAction::triggered, [=]()
    {
        QModelIndex idx = m_ClientsView->currentIndex();

        if (idx.isValid())
        {
            QModelIndex sourceindex = m_FilterModel->mapToSource(idx);
            quint32 id = m_ClientsModel->data(m_ClientsModel->index(sourceindex.row(), ClientsViewModel::FldID)).toInt();

            PersonPanel dlg(id);
            dlg.setViewMode();
            dlg.exec();
        }
    });

    connect(m_ClientsView->selectionModel(), &QItemSelectionModel::currentChanged, [=](const QModelIndex &current, const QModelIndex &previous)
    {
        Q_UNUSED(previous);

        if (current.isValid())
        {
            const QAbstractItemModel * const model = current.model();
            m_pViewParty->setEnabled(true);
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

            m_Result["id"] = model->data(model->index(current.row(), ClientsViewModel::FldID)).toInt();
            m_Result["name"] = model->data(model->index(current.row(), ClientsViewModel::FldFullName));
        }
        else
        {
            m_pViewParty->setEnabled(false);
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            m_Result.clear();
        }
    });

    connect(m_ClientsView.data(), &QListView::customContextMenuRequested, [=](const QPoint &pt)
    {
        QPoint pos = m_ClientsView->mapToGlobal(pt);
        QMenu::exec({m_pNewParty, m_pViewParty}, pos);
    });

    connect(m_ClientsView.data(), &QListView::doubleClicked, [=](const QModelIndex &index)
    {
        if (index.isValid())
            accept();
    });
}

SelectPartyDlg::~SelectPartyDlg()
{
    delete ui;
}

const SelectPartyResult &SelectPartyDlg::result() const
{
    return m_Result;
}
