#include "renovationpanel.h"
#include "ui_renovationpanel.h"

RenovationPanel::RenovationPanel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenovationPanel)
{
    ui->setupUi(this);
}

RenovationPanel::~RenovationPanel()
{
    delete ui;
}
