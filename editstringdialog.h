#ifndef EDITSTRINGDIALOG_H
#define EDITSTRINGDIALOG_H

#include <QDialog>

namespace Ui {
class EditStringDialog;
}

class QLineEdit;
class QPushButton;
class EditStringDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditStringDialog(QWidget *parent = nullptr);
    ~EditStringDialog();

    void setText(const QString &value);
    QString text() const;

public slots:
    virtual void accept();

private slots:
    void textEdited(const QString &value);

protected:
    virtual bool onSaveInput();
    virtual void checkInput(const QString &value);
    void setCustomLineEdit(QLineEdit *linedit);
    void setEnabledOk(bool val);
    bool okButtonState();
    QLineEdit *edit() const;

private:
    Ui::EditStringDialog *ui;
    QString m_SaveValue;
    QPushButton *m_OkButton;
};

#endif // EDITSTRINGDIALOG_H
