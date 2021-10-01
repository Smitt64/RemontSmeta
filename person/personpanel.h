#ifndef PERSONPANEL_H
#define PERSONPANEL_H

#include <QDialog>
#include "person/tpersonservice.h"
#include "db/dbtable.h"
#include <QStringListModel>

namespace Ui {
class PersonPanel;
}

class QDataWidgetMapper;
class ContactsProxy;
class AdressProxy;
class QShortcut;
class QListWidgetItem;
class PersonPanel : public QDialog
{
    Q_OBJECT

public:
    explicit PersonPanel(const quint16 &person = 0, QWidget *parent = nullptr);
    ~PersonPanel();

private slots:
    void onSave();
    void contactsContextMenu(const QPoint &pos);
    void adressContextMenu(const QPoint &pos);
    void contactsDoubleClicked(const QModelIndex &index);
    void adressDoubleClicked(const QModelIndex &index);
    void updateFullName();
    void inputPhone();
    void inputEmail();
    void inputAdress();
    void editContact(const quint16 &row);
    void editAdress(const quint16 &row);
    void ownChanged(QListWidgetItem *item);

private:
    void setupMapping();
    void setupShortcuts();
    void pumpOwns();

    QScopedPointer<QDataWidgetMapper> m_DataMapper;
    Ui::PersonPanel *ui;

    QScopedPointer<ContactsProxy> m_ContactsProxy;
    QScopedPointer<AdressProxy> m_AdressProxy;
    QSharedPointer<DbTable> m_Table, m_Contacts, m_Adress, m_PersonOwn;
    QScopedPointer<TPersonService> m_Service;

    QScopedPointer<QShortcut> m_AddPhoneShortcut, m_AddEmailShortcut, m_AddAdressShortcut;

    //TestModel tmpmodel;
};

#endif // PERSONPANEL_H
