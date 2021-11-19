#include "renovationpanel.h"
#include "renovationcontentwidget.h"
#include "ui_renovationpanel.h"
#include "globals.h"
#include "models/jsontablemodel.h"
#include "widgets/selectpartydlg.h"
#include <QAction>

RenovationPanel::RenovationPanel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenovationPanel)
{
    ui->setupUi(this);
    m_ContentWidget.reset(new RenovationContentWidget(0));
    ui->verticalLayout->addWidget(m_ContentWidget.data());

    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
}


RenovationPanel::~RenovationPanel()
{
    delete ui;
}



// =====================================================

/*RenovationWindow::RenovationWindow() :
    QMainWindow()
{
    m_pPanel.reset(new RenovationPanel());
    setCentralWidget(m_pPanel.data());
}
*/
