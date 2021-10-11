#include "viewpartywindow.h"
#include "clientswindow/clientsviewmodel.h"
#include "tables/tclient.h"
#include "person/personpanel.h"
#include <QMainWindow>
#include <QSplitter>
#include <QTableView>
#include <QDebug>
#include <QTreeView>
#include <QToolBar>
#include <QEnterEvent>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QMessageBox>
#include <QMenu>
#include <QStyle>

class GenderColumnDlegate : public QStyledItemDelegate
{
public:
    GenderColumnDlegate(QObject *parent = nullptr) :
        QStyledItemDelegate(parent)
    {

    }

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE
    {
        if (index.column() != ClientsViewModel::FldSex)
            QStyledItemDelegate::paint(painter, option, index);
        else
        {
            QIcon icon = index.model()->data(index, Qt::DecorationRole).value<QIcon>();

            if (!icon.isNull())
            {
                if (option.state & QStyle::State_Selected)
                {
                    QImage image = icon.pixmap(option.rect.size()).toImage();
                    image.invertPixels();
                    icon = QIcon(QPixmap::fromImage(image));
                }
                const QStyle *style = option.widget ? option.widget->style() : qApp->style();
                style->drawItemPixmap(painter, option.rect, Qt::AlignCenter, icon.pixmap(option.rect.size()));
            }
        }
    }
};

// ----------------------------------------------------------------------------------------------------

ViewPartyWindow::ViewPartyWindow(QWidget *parent) :
    SubWindowBase(parent)
{
    m_Splitter.reset(new QSplitter(Qt::Vertical));

    setupClientsList();
    setContentWidget(m_Splitter.data());

    setWindowTitle(tr("Все субъекты"));
    setWindowIcon(QIcon::fromTheme("Team"));
}

ViewPartyWindow::~ViewPartyWindow()
{
}

void ViewPartyWindow::setupClientsList()
{
    m_ClientsWindow.reset(new QMainWindow());
    m_ClientsList.reset(new QTableView());
    m_GenderDelegate.reset(new GenderColumnDlegate());
    m_Splitter->addWidget(m_ClientsWindow.data());

    m_ClientsList->verticalHeader()->setDefaultSectionSize(20);
    m_ClientsList->horizontalHeader()->setHighlightSections(false);
    m_ClientsList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ClientsList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ClientsList->setContextMenuPolicy(Qt::CustomContextMenu);
    m_ClientsWindow->setCentralWidget(m_ClientsList.data());

    m_ClientsToolBar.reset(new QToolBar());
    m_ClientsToolBar->setIconSize(QSize(16, 16));
    m_ClientsWindow->addToolBar(Qt::TopToolBarArea, m_ClientsToolBar.data());

    m_PartyActions.append(m_ClientsToolBar->addAction(QIcon::fromTheme("NewTeam"), tr("Добавить субъекта")));
    m_ClientsToolBar->addSeparator();
    m_PartyActions.append(m_ClientsToolBar->addAction(QIcon::fromTheme("ViewTeam"), tr("Просмотр в новой вкладке")));
    m_ClientsToolBar->addSeparator();
    m_PartyActions.append(m_ClientsToolBar->addAction(QIcon::fromTheme("EditTeam"), tr("Редактировать субъекта")));
    m_PartyActions.append(m_ClientsToolBar->addAction(QIcon::fromTheme("RemoveTeam"), tr("Удалить субъекта")));

    m_PartyActions[PA_Edit]->setStatusTip(m_PartyActions[PA_Edit]->text());
    m_PartyActions[PA_Remove]->setStatusTip(m_PartyActions[PA_Remove]->text());

    updateActionsEnable(false);

    connect(m_ClientsList.data(), &QAbstractItemView::doubleClicked, this, &ViewPartyWindow::partyDoubleClicked);
    connect(m_ClientsList.data(), &QAbstractItemView::customContextMenuRequested, this, &ViewPartyWindow::partyContextMenuRequested);
    connect(m_PartyActions[PA_Create], &QAction::triggered, this, &ViewPartyWindow::addParty);
    connect(m_PartyActions[PA_Remove], &QAction::triggered, this, &ViewPartyWindow::removePartyAction);
    connect(m_PartyActions[PA_Edit], &QAction::triggered, this, &ViewPartyWindow::editPerson);
    connect(m_PartyActions[PA_ViewInTab], &QAction::triggered, this, &ViewPartyWindow::openParty);
}

QSplitter *ViewPartyWindow::splitter() const
{
    return m_Splitter.data();
}

ClientsViewModel *ViewPartyWindow::clientsModel()
{
    return m_ClientsModel.data();
}

void ViewPartyWindow::initModel()
{

}

void ViewPartyWindow::showEvent(QShowEvent *event)
{
    if (!m_ClientsModel)
    {
        m_ClientsModel.reset(new ClientsViewModel());
        initModel();
        m_ClientsModel->resetQuery();
        m_ClientsList->setModel(m_ClientsModel.data());

        m_ClientsList->setColumnWidth(ClientsViewModel::FldFullName, 250);
        m_ClientsList->setColumnWidth(ClientsViewModel::FldSex, 35);
        m_ClientsList->setColumnWidth(ClientsViewModel::FldPhone, 110);
        m_ClientsList->setColumnWidth(ClientsViewModel::FldEmail, 150);
        m_ClientsList->setColumnWidth(ClientsViewModel::FldAdress, 250);

        m_ClientsList->hideColumn(ClientsViewModel::FldID);
        m_ClientsList->setItemDelegateForColumn(ClientsViewModel::FldSex, m_GenderDelegate.data());

        m_ClientsList->horizontalHeader()->setSectionResizeMode(ClientsViewModel::FldSex, QHeaderView::Fixed);

        connect(m_ClientsList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ViewPartyWindow::partySelectionChanged);
    }
    SubWindowBase::showEvent(event);
}

void ViewPartyWindow::editPerson()
{
    QModelIndex index = m_ClientsList->currentIndex();
    partyDoubleClicked(index);
}

void ViewPartyWindow::partyDoubleClicked(const QModelIndex &index)
{
    if (index.isValid())
    {
        quint16 id = m_ClientsModel->data(m_ClientsModel->index(index.row(), ClientsViewModel::FldID)).value<quint16>();

        PersonPanel dlg(id, this);
        if (dlg.exec() == QDialog::Accepted)
        {
            m_ClientsModel->resetQuery();
            QModelIndexList indeces = m_ClientsModel->match(m_ClientsModel->index(0, ClientsViewModel::FldID), Qt::DisplayRole, id, 1, Qt::MatchFixedString);

            if (!indeces.isEmpty())
            {
                m_ClientsList->scrollTo(indeces.first());
                m_ClientsList->setCurrentIndex(indeces.first());
            }
            //m_ClientsList->setCurrentIndex(index);
        }
    }
}

void ViewPartyWindow::addParty()
{
    PersonPanel dlg(0, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        m_ClientsModel->resetQuery();
        //m_ClientsList->setCurrentIndex(index);
    }
}

void ViewPartyWindow::removePartyAction()
{
    QModelIndex index = m_ClientsList->currentIndex();

    if (index.isValid())
    {
        QString name = m_ClientsModel->data(m_ClientsModel->index(index.row(), ClientsViewModel::FldFullName)).toString();
        quint16 id = m_ClientsModel->data(m_ClientsModel->index(index.row(), ClientsViewModel::FldID)).value<quint16>();

        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Удаление"), tr("Удалить субъекта <b>\"%1\"<\b>?").arg(name));
        if (button == QMessageBox::Yes)
        {
            bool hr = TPerson::deletePerson(id);

            if (hr)
            {
                m_ClientsModel->resetQuery();

                index = m_ClientsList->currentIndex();

                if (!index.isValid())
                    updateActionsEnable(false);
            }
        }
    }
}

void ViewPartyWindow::updateActionsEnable(const bool &val)
{
    m_PartyActions[PA_Edit]->setEnabled(val);
    m_PartyActions[PA_Remove]->setEnabled(val);
    m_PartyActions[PA_ViewInTab]->setEnabled(val);
}

void ViewPartyWindow::partySelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    bool enable = false;
    if (selected.count() && selected.indexes().at(0).isValid())
    {
        QModelIndex index = selected.indexes().at(0);
        QString name = m_ClientsModel->data(m_ClientsModel->index(index.row(), ClientsViewModel::FldFullName)).toString();
        enable = true;

        setActionTextAndStatus(m_PartyActions[PA_Edit], tr("Редактировать: %1").arg(name));
        setActionTextAndStatus(m_PartyActions[PA_Remove], tr("Удалить: %1").arg(name));
        setActionTextAndStatus(m_PartyActions[PA_ViewInTab], tr("Просмотр в новой вкладке: %1").arg(name));
    }

    updateActionsEnable(enable);
}

void ViewPartyWindow::createActionSeparator(QAction **action)
{
    *action = new QAction();
    (*action)->setSeparator(true);
}

void ViewPartyWindow::partyContextMenuRequested(const QPoint &pos)
{
    QPoint mapped = m_ClientsList->viewport()->mapToGlobal(pos);
    QModelIndex index = m_ClientsList->indexAt(pos);

    QList<QAction*> actions, actionsToDelete;
    actions.append(m_PartyActions[PA_Create]);

    QString name;
    quint16 id = 0;

    if (index.isValid())
    {
        name = m_ClientsModel->data(m_ClientsModel->index(index.row(), ClientsViewModel::FldFullName)).toString();
        id = m_ClientsModel->data(m_ClientsModel->index(index.row(), ClientsViewModel::FldID)).value<quint16>();
        actions.append(m_PartyActions[PA_ViewInTab]);

        QAction *sep1 = nullptr;
        createActionSeparator(&sep1);

        actions.append(sep1);
        actionsToDelete.append(sep1);
        actions.append(m_PartyActions[PA_Edit]);
        actions.append(m_PartyActions[PA_Remove]);
        /*QAction *action = new QAction(QIcon::fromTheme("EditTeam"), tr("Редактировать: %1").arg(name));
        actions.append(action);
        actionsToDelete.append(action);*/
    }

    QAction *action = QMenu::exec(actions, mapped);

    /*if (action == m_pAddParty)
        m_pAddParty->trigger();*/
}

void ViewPartyWindow::openParty()
{
    QModelIndex index = m_ClientsList->currentIndex();

    if (index.isValid())
    {
        quint16 id = m_ClientsModel->data(m_ClientsModel->index(index.row(), ClientsViewModel::FldID)).value<quint16>();
        PersonPanel *panel = new PersonPanel(id);
        panel->setViewMode();

        emit requestAddSubWindow(panel);
    }
}

// ----------------------------------------------------------------------------------------------------

ViewClientsWindow::ViewClientsWindow(QWidget *parent) :
    ViewPartyWindow(parent)
{
    setWindowTitle(tr("Список клиентов"));
    setupWorsksList();
}

ViewClientsWindow::~ViewClientsWindow()
{

}

void ViewClientsWindow::setupWorsksList()
{
    m_WorksWindow.reset(new QMainWindow());
    m_WorksList.reset(new QTreeView());
    splitter()->addWidget(m_WorksWindow.data());

    m_WorksList->setRootIsDecorated(false);
    m_WorksList->setUniformRowHeights(true);
    m_WorksWindow->setCentralWidget(m_WorksList.data());
}

void ViewClientsWindow::initModel()
{
    clientsModel()->setPartyOwns({TPersonOwn::OwnClient});
}
