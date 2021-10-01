#include "mainwindow.h"
#include "person/personpanel.h"
#include "navigationdockwidget.h"
#include <QMdiArea>
#include <QMenuBar>
#include <QMenu>
#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_pClientsDock(nullptr)
{
    m_pMdiArea = new QMdiArea(this);
    m_pMdiArea->setViewMode(QMdiArea::TabbedView);
    m_pMdiArea->setTabsClosable(true);
    setCentralWidget(m_pMdiArea);
    setWindowIcon(QIcon::fromTheme("Address_16x"));

    setupDocks();
    setupMenus();
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

    //connect(m_pClientsDock, &NavigationDockWidget::itemClicked, this, &MainWindow::navigationClicked);
    connect(m_pClientsDock, SIGNAL(itemClicked(QString)), this, SLOT(navigationClicked(QString)));
}

void MainWindow::setupMenus()
{
    m_MainMenuBar = new QMenuBar(this);
    m_DictionaryMenu = m_MainMenuBar->addMenu(tr("Справочники"));

    QAction *addPerson = m_DictionaryMenu->addAction(tr("Добавить клиента"));

    setMenuBar(m_MainMenuBar);

    connect(addPerson, &QAction::triggered, this, &MainWindow::addClientAction);
}

void MainWindow::addClientAction()
{
    QScopedPointer<PersonPanel> panel(new PersonPanel(5, this));
    panel->exec();
}

void MainWindow::navigationClicked(const QString &mime)
{
    QMdiSubWindow *wnd = m_pMdiArea->addSubWindow(new QWidget());
    wnd->show();
}
