#include "mainwindow.h"
#include "person/personpanel.h"
#include "models/subwindowsmodel.h"
#include "navigationdockwidget.h"
#include "globals.h"
#include "subwindowbase.h"
#include <QMdiArea>
#include <QMenuBar>
#include <QApplication>
#include <QMenu>
#include <QMdiSubWindow>
#include <QStatusBar>
#include <QComboBox>
#include <QSizePolicy>
#include <QWidgetAction>

class SubWindowsMenu : public QWidgetAction
{
public:
    SubWindowsMenu(SubWindowsModel *model) :
        QWidgetAction (model),
        m_pModel(model)
    {

    }

    virtual QWidget *createWidget(QWidget *parent) Q_DECL_OVERRIDE
    {
        QComboBox *combo = new QComboBox(parent);
        QPalette palette = combo->palette();

        combo->setFixedWidth(200);
        combo->setModel(m_pModel);
        return combo;
    }

    virtual void deleteWidget(QWidget *widget) Q_DECL_OVERRIDE
    {
        delete widget;
    }

private:
    SubWindowsModel *m_pModel;
};

// ----------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_pClientsDock(nullptr)
{
    m_pMdiArea = new QMdiArea(this);
    m_pMdiArea->setViewMode(QMdiArea::TabbedView);
    m_pMdiArea->setDocumentMode(true);
    m_pMdiArea->setTabsClosable(true);
    m_pMdiArea->setTabsMovable(true);
    m_pMdiArea->setOption(QMdiArea::DontMaximizeSubWindowOnActivation, false);
    setCentralWidget(m_pMdiArea);
    setWindowIcon(QIcon::fromTheme("Repair"));

    setupMenus();
    setupDocks();
    setupStatusBar();
    setupWindowsList();
}

MainWindow::~MainWindow()
{
    if (m_pClientsDock)
        delete m_pClientsDock;
}

void MainWindow::setupDocks()
{
    m_pClientsDock = new NavigationDockWidget;

    addDockWidget(Qt::LeftDockWidgetArea, m_pClientsDock);

    m_ViewMenu->addAction(m_pClientsDock->toggleViewAction());

    //connect(m_pClientsDock, &NavigationDockWidget::itemClicked, this, &MainWindow::navigationClicked);
    connect(m_pClientsDock, SIGNAL(itemClicked(QString)), this, SLOT(navigationClicked(QString)));
}

void MainWindow::setupMenus()
{
    m_MainMenuBar = new QMenuBar(this);
    m_ViewMenu = m_MainMenuBar->addMenu(tr("Вид"));
    m_DictionaryMenu = m_MainMenuBar->addMenu(tr("Справочники"));

    QAction *addPerson = m_DictionaryMenu->addAction(tr("Добавить клиента"));

    setMenuBar(m_MainMenuBar);

    connect(addPerson, &QAction::triggered, this, &MainWindow::addClientAction);
}

void MainWindow::setupStatusBar()
{
    m_pStatusBar = new QStatusBar(this);
    setStatusBar(m_pStatusBar);
}

void MainWindow::setupWindowsList()
{
    m_WindowsModel.reset(new SubWindowsModel());
    m_WindowsMenu.reset(new QMenu());
    m_WindowsMenu->addAction(new SubWindowsMenu(m_WindowsModel.data()));
    menuBar()->setCornerWidget(m_WindowsMenu.data());

    menuBar()->setNativeMenuBar(false);
    //menuBar()->setCornerWidget(m_WindowsComboBox.data(), Qt::TopLeftCorner);

    menuBar()->adjustSize();
}

void MainWindow::addClientAction()
{
    QScopedPointer<PersonPanel> panel(new PersonPanel(5, this));
    panel->exec();
}

void MainWindow::addWindowPrivate(SubWindowBase *wnd)
{
    if (wnd)
    {
        m_WindowsModel->addWindow(wnd);
        connect(wnd, &SubWindowBase::requestAddSubWindow, this, &MainWindow::subWindowRequestNewWindow);
        wnd->show();
    }
}

void MainWindow::navigationClicked(const QString &mime)
{
    SubWindowBase *wnd = Globals::inst()->create(mime, m_pMdiArea);
    addWindowPrivate(wnd);
}

void MainWindow::subWindowRequestNewWindow(QWidget *widget)
{
    SubWindowBase *wnd = new SubWindowBase(m_pMdiArea);
    wnd->setContentWidget(widget);
    addWindowPrivate(wnd);
}
