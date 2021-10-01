#include "editcontactdlg.h"
#include "globals.h"
#include "adresslineedit.h"
#include <QLineEdit>
#include <QIcon>
#include <QMessageBox>
#include <QRegularExpression>
#include <QCompleter>

EditContactDlg::EditContactDlg(QWidget *parent) :
    EditStringDialog(parent)
{
    setupPanel();
}

EditContactDlg::EditContactDlg(const QString &value, QWidget *parent) :
    EditStringDialog(parent)
{
    setText(value);
    setupPanel();
}

EditContactDlg::~EditContactDlg()
{

}

void EditContactDlg::setupPanel()
{
    if (edit()->text().isEmpty())
    {
        setWindowTitle(tr("Новый телефон"));
        setWindowIcon(QIcon::fromTheme("NewPhone"));
    }
    else
    {
        setWindowTitle(tr("Редактировать телефон"));
        setWindowIcon(QIcon::fromTheme("EditPhone"));
    }

    edit()->setInputMask("[+]9-\\(999\\)-999-99-99;#");
}

bool EditContactDlg::onSaveInput()
{
    bool hr = EditStringDialog::onSaveInput();

    if (!hr)
        QMessageBox::warning(this, tr("Предупреждение"), tr("Телефон не указан"));

    return hr;
}

// ----------------------------------------------------------------------------------------

EditEmailDlg::EditEmailDlg(QWidget *parent) :
    EditStringDialog(parent)
{
    setupPanel();
}

EditEmailDlg::EditEmailDlg(const QString &value, QWidget *parent) :
    EditStringDialog(parent)
{
    setText(value);
    setupPanel();

    //connect(this, &EditStringDialog::textChanged, this, &EditEmailDlg::textEditedValidate);
}

EditEmailDlg::~EditEmailDlg()
{

}

void EditEmailDlg::setupPanel()
{
    m_Original = edit()->palette();
    m_Invalid = m_Original;
    m_Invalid.setColor(QPalette::Text, Qt::red);

    if (edit()->text().isEmpty())
    {
        setWindowTitle(tr("Новый электронный адрес"));
        setWindowIcon(QIcon::fromTheme("NewEmailAddress"));
    }
    else
    {
        setWindowTitle(tr("Редактировать электронный адрес"));
        setWindowIcon(QIcon::fromTheme("EditEmailAddress"));
    }

    edit()->setPlaceholderText("example@mail.com");
}

bool EditEmailDlg::onSaveInput()
{
    bool hr = EditStringDialog::onSaveInput();

    if (!hr)
        QMessageBox::warning(this, tr("Предупреждение"), tr("Телефон не указан"));

    return hr;
}

void EditEmailDlg::checkInput(const QString &value)
{
    bool result = okButtonState();
    bool hasMatch = validateEmail(value);

    if (hasMatch)
    {
        edit()->setPalette(m_Original);
        setEnabledOk(result);
    }
    else
    {
        setEnabledOk(false);
        edit()->setPalette(m_Invalid);
    }
}

bool EditEmailDlg::validateEmail(const QString &value)
{
    QRegularExpression regex("^[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}$");
    QRegularExpressionMatch match = regex.match(value);

    bool hasMatch = match.hasMatch();
    return hasMatch;
}

// ----------------------------------------------------------------------------------------

EditAddressDlg::EditAddressDlg(QWidget *parent) :
    EditStringDialog(parent)
{
    setupPanel();
}

EditAddressDlg::EditAddressDlg(const QString &value, QWidget *parent) :
    EditStringDialog(parent)
{
    setText(value);
    setupPanel();
}

EditAddressDlg::~EditAddressDlg()
{

}

void EditAddressDlg::setupPanel()
{
    m_AderessEdit = new AdressLineEdit();
    setCustomLineEdit(m_AderessEdit);
    m_AderessEdit->setPlaceholderText(tr("Тверская улица, дом 13"));

    if (edit()->text().isEmpty())
    {
        setWindowTitle(tr("Новый адрес"));
        setWindowIcon(QIcon::fromTheme("NewAddress"));
    }
    else
    {
        setWindowTitle(tr("Редактировать адрес"));
        setWindowIcon(QIcon::fromTheme("EditAddress"));
    }
}

bool EditAddressDlg::onSaveInput()
{
    bool hr = EditStringDialog::onSaveInput();

    if (!hr)
        QMessageBox::warning(this, tr("Предупреждение"), tr("Телефон не указан"));

    return hr;
}
