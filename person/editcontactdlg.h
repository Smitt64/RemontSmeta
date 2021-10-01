#ifndef EDITCONTACTDLG_H
#define EDITCONTACTDLG_H

#include <editstringdialog.h>
#include <QObject>

class EditContactDlg : public EditStringDialog
{
    Q_OBJECT
public:
    EditContactDlg(QWidget *parent = nullptr);
    EditContactDlg(const QString &value, QWidget *parent = nullptr);
    virtual ~EditContactDlg();

protected:
    virtual bool onSaveInput() Q_DECL_OVERRIDE;

private:
    void setupPanel();
};

// -------------------------------------------------------------------------

class EditEmailDlg : public EditStringDialog
{
    Q_OBJECT
public:
    EditEmailDlg(QWidget *parent = nullptr);
    EditEmailDlg(const QString &value, QWidget *parent = nullptr);
    virtual ~EditEmailDlg();

protected:
    virtual bool onSaveInput() Q_DECL_OVERRIDE;
    virtual void checkInput(const QString &value) Q_DECL_OVERRIDE;

private:
    void setupPanel();
    bool validateEmail(const QString &value);

    QPalette m_Original, m_Invalid;
};

// -------------------------------------------------------------------------

class QCompleter;
class AdressLineEdit;
class EditAddressDlg : public EditStringDialog
{
    Q_OBJECT
public:
    EditAddressDlg(QWidget *parent = nullptr);
    EditAddressDlg(const QString &value, QWidget *parent = nullptr);
    virtual ~EditAddressDlg();

protected:
    virtual bool onSaveInput() Q_DECL_OVERRIDE;

private:
    void setupPanel();
    AdressLineEdit *m_AderessEdit;
};

#endif // EDITCONTACTDLG_H
