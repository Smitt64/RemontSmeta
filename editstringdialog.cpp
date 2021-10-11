#include "editstringdialog.h"
#include "ui_editstringdialog.h"
#include <QDialogButtonBox>
#include <QPushButton>

EditStringDialog::EditStringDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditStringDialog)
{
    ui->setupUi(this);
    setSizeGripEnabled(false);
    setFixedSize(QSize(350, 70));

    m_OkButton = ui->buttonBox->button(QDialogButtonBox::QDialogButtonBox::Ok);
    m_OkButton->setEnabled(false);

    connect(ui->lineEdit, &QLineEdit::textEdited, this, &EditStringDialog::textEdited);
}

EditStringDialog::~EditStringDialog()
{
    delete ui;
}

void EditStringDialog::setCustomLineEdit(QLineEdit *linedit)
{
    QString oldValue = ui->lineEdit->text();
    ui->verticalLayout->removeWidget(ui->lineEdit);
    delete ui->lineEdit;

    ui->verticalLayout->insertWidget(0, linedit);
    ui->lineEdit = linedit;
    ui->lineEdit->setText(oldValue);
    connect(ui->lineEdit, &QLineEdit::textEdited, this, &EditStringDialog::textEdited);
}

QLineEdit *EditStringDialog::edit() const
{
    return ui->lineEdit;
}

bool EditStringDialog::onSaveInput()
{
    bool hr = true;

    if (ui->lineEdit->text().isEmpty())
        hr = false;

    return hr;
}

void EditStringDialog::accept()
{
    if (onSaveInput())
        QDialog::accept();
}

void EditStringDialog::setText(const QString &value)
{
    ui->lineEdit->setText(value);
    m_SaveValue = ui->lineEdit->text();
    setEnabledOk(false);
}

void EditStringDialog::checkInput(const QString &value)
{
    Q_UNUSED(value);
}

QString EditStringDialog::text() const
{
    return ui->lineEdit->text();
}

bool EditStringDialog::okButtonState()
{
    bool result = false;

    if (text() == m_SaveValue || !ui->lineEdit->hasAcceptableInput())
        result = false;
    else
    {
        if (text().isEmpty())
            result = false;
        else
            result = true;
    }

    return result;
}

void EditStringDialog::textEdited(const QString &value)
{
    Q_UNUSED(value);

    bool result = okButtonState();
    setEnabledOk(result);

    checkInput(value);
}

void EditStringDialog::setEnabledOk(bool val)
{
    m_OkButton->setEnabled(val);
}

void EditStringDialog::setReadOnly(bool value)
{
    ui->lineEdit->setReadOnly(value);
    ui->buttonBox->setVisible(!value);
}
