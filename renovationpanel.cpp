#include "renovationpanel.h"
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

    m_pTypeModel = Globals::inst()->model(JSON_RENOVATION_TYPE);
    m_PromotionsModel = Globals::inst()->model(JSON_PROMOTIONS);
    m_pHouseModel = Globals::inst()->model(JSON_BUILDTYPE);

    ui->typeComboBox->setModel(m_pTypeModel);
    ui->typeComboBox->setModelColumn(1);

    ui->promotionsBox->setModel(m_PromotionsModel);
    ui->promotionsBox->setModelColumn(1);

    ui->houseComboBox->setModel(m_pHouseModel);
    ui->houseComboBox->setModelColumn(1);

    connect(ui->clientButton, &QToolButton::clicked, this, &RenovationPanel::onClientsClick);
}

RenovationPanel::~RenovationPanel()
{
    delete ui;
}

void RenovationPanel::onClientsClick()
{
    SelectPartyDlg dlg(this);
    dlg.exec();
}
