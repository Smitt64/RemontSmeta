#include "personpanel.h"
#include "ui_personpanel.h"
#include "editcontactdlg.h"
#include "dbtablebaseproxymodel.h"
#include "globals.h"
#include "jsclasses/jsclassexecuter.h"
#include <QDataWidgetMapper>
#include <QListWidgetItem>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QShortcut>
#include <QKeySequence>
#include <QMessageBox>
#include <QCompleter>

template<class T>
void setReadOnly(QScopedPointer<T> &widget)
{
    if (std::is_member_function_pointer<decltype(&T::setEnabled)>::value)
        widget->setEnabled(false);
}

template<class T>
void setReadOnly(T *widget)
{
    if constexpr (std::is_same<T, QLineEdit>::value || std::is_same<T, QPlainTextEdit>::value)
        widget->setReadOnly(true);

    if constexpr(std::is_same<T, QComboBox>::value)
        widget->setEnabled(false);

    if constexpr(std::is_same<T, QListView>::value || std::is_same<T, QListWidget>::value)
    {
        widget->setContextMenuPolicy(Qt::NoContextMenu);
        widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    if constexpr(std::is_same<T, QListWidget>::value)
        widget->setEnabled(false);
}

// -------------------------------------------------------------------------

class ContactsProxy : public DbTableBaseProxyModel
{
public:
    ContactsProxy(DbTable *table) :
        DbTableBaseProxyModel(table)
    {

    }

    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_FINAL
    {
        return index(sourceIndex.row(), 0, QModelIndex());
    }

    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_FINAL
    {
        return index(proxyIndex.row(), table()->fieldNum("t_value"), QModelIndex());
    }

    virtual int columnCount(const QModelIndex &parent) const Q_DECL_FINAL
    {
        Q_UNUSED(parent);
        return 1;
    }

    virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
    {
        if (role == Qt::DecorationRole && table()->seek(proxyIndex.row()))
        {
            QVariant val;
            quint16 type = table()->value("t_type").value<quint16>();
            switch (type)
            {
            case TContact::TypePhone:
                val = QIcon::fromTheme("Phone");
                break;
            case TContact::TypeEmail:
                val = QIcon::fromTheme("EmailAddress");
                break;
            }

            return val;
        }
        return DbTableBaseProxyModel::data(proxyIndex, role);
    }
};

// -------------------------------------------------------------------------

class AdressProxy : public DbTableBaseProxyModel
{
public:
    AdressProxy(DbTable *table) :
        DbTableBaseProxyModel(table)
    {

    }

    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_FINAL
    {
        return index(sourceIndex.row(), 0, QModelIndex());
    }

    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_FINAL
    {
        return index(proxyIndex.row(), table()->fieldNum("t_address"), QModelIndex());
    }

    virtual int columnCount(const QModelIndex &parent) const Q_DECL_FINAL
    {
        Q_UNUSED(parent);
        return 1;
    }

    virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
    {
        if (role == Qt::DecorationRole && table()->seek(proxyIndex.row()))
            return QIcon::fromTheme("Address");

        return DbTableBaseProxyModel::data(proxyIndex, role);
    }
};

// -------------------------------------------------------------------------

PersonPanel::PersonPanel(const quint16 &person, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PersonPanel)
{
    ui->setupUi(this);
    m_NameCompleter.reset(new QCompleter());
    m_NameCompleter->setModel(Globals::inst()->namesModel());
    m_NameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_NameCompleter->setFilterMode(Qt::MatchContains);
    ui->nameEdit->setCompleter(m_NameCompleter.data());

    m_Service.reset(new TPersonService(person));
    m_Service->start();

    m_Table = m_Service->createTableObj();
    m_Contacts = m_Service->contactsTable();
    m_Adress = m_Service->adressTable();
    m_PersonOwn = m_Service->ownTable();

    if (person)
    {
        m_Table->find((quint16)0, QVariantList() << person);
        m_Contacts->find((quint16)1, QVariantList() << person);
        m_Adress->find((quint16)1, QVariantList() << person);
        setWindowTitle(tr("Редактирование субъекта"));
        setWindowIcon(QIcon::fromTheme("Team"));
    }
    else
    {
        if (m_Table->newRec() && m_Table->insert())
        {
            setWindowTitle(tr("Ввод нового субъекта"));
            setWindowIcon(QIcon::fromTheme("NewTeam"));
        }
    }

    ui->sexBox->setItemIcon(1, QIcon::fromTheme("male"));
    ui->sexBox->setItemIcon(2, QIcon::fromTheme("famele"));
    ui->contactsView->setContextMenuPolicy(Qt::CustomContextMenu);

    setupMapping();
    setupShortcuts();
    setupJsEnbgine();
    pumpOwns();

    connect(this, &QDialog::accepted, this, &PersonPanel::onSave);
    connect(ui->contactsView, &QWidget::customContextMenuRequested, this, &PersonPanel::contactsContextMenu);
    connect(ui->contactsView, &QAbstractItemView::doubleClicked, this, &PersonPanel::contactsDoubleClicked);

    connect(ui->adressView, &QWidget::customContextMenuRequested, this, &PersonPanel::adressContextMenu);
    connect(ui->adressView, &QAbstractItemView::doubleClicked, this, &PersonPanel::adressDoubleClicked);

    connect(ui->familyEdit, &QLineEdit::editingFinished, this, &PersonPanel::updateFullName);
    connect(ui->nameEdit, &QLineEdit::editingFinished, this, &PersonPanel::updateFullName);
    connect(ui->patronymicEdit, &QLineEdit::editingFinished, this, &PersonPanel::updateFullName);
    connect(ui->ownWidget, &QListWidget::itemChanged, this, &PersonPanel::ownChanged);
}

PersonPanel::~PersonPanel()
{
    //m_DataMapper.reset();
    delete ui;
}

void PersonPanel::setupMapping()
{
    m_DataMapper.reset(new QDataWidgetMapper());
    m_DataMapper->setModel(m_Table.data());

    m_DataMapper->addMapping(ui->familyEdit, m_Table->fieldNum("t_family"));
    m_DataMapper->addMapping(ui->nameEdit, m_Table->fieldNum("t_name"));
    m_DataMapper->addMapping(ui->patronymicEdit, m_Table->fieldNum("t_patronymic"));
    m_DataMapper->addMapping(ui->fullnameEdit, m_Table->fieldNum("t_fullname"));
    m_DataMapper->addMapping(ui->sexBox, m_Table->fieldNum("t_sex"), "currentIndex");
    m_DataMapper->addMapping(ui->noteEdit, m_Table->fieldNum("t_note"), "plainText");

    m_DataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_DataMapper->toFirst();

    m_ContactsProxy.reset(new ContactsProxy(m_Contacts.data()));
    m_ContactsProxy->setSourceModel(m_Contacts.data());
    ui->contactsView->setModel(m_ContactsProxy.data());

    m_AdressProxy.reset(new AdressProxy(m_Adress.data()));
    m_AdressProxy->setSourceModel(m_Adress.data());
    ui->adressView->setModel(m_AdressProxy.data());
    ui->adressView->setIconSize(QSize(16, 16));
}

void PersonPanel::onSave()
{
    m_Table->submit();
    m_Service->sync();
}

void PersonPanel::setupShortcuts()
{
    m_AddPhoneShortcut.reset(new QShortcut(QKeySequence(Qt::CTRL KEY_OP Qt::Key_P), this));
    m_AddEmailShortcut.reset(new QShortcut(QKeySequence(Qt::CTRL KEY_OP Qt::Key_E), this));
    m_AddAdressShortcut.reset(new QShortcut(QKeySequence(Qt::CTRL KEY_OP Qt::Key_R), this));

    connect(m_AddPhoneShortcut.data(), &QShortcut::activated, this, &PersonPanel::inputPhone);
    connect(m_AddEmailShortcut.data(), &QShortcut::activated, this, &PersonPanel::inputEmail);
    connect(m_AddAdressShortcut.data(), &QShortcut::activated, this, &PersonPanel::inputAdress);
}

void PersonPanel::updateFullName()
{
    QString fullname;

    if (!ui->familyEdit->text().isEmpty())
        fullname = ui->familyEdit->text();

    if (!ui->nameEdit->text().isEmpty())
    {
        if (!fullname.isEmpty())
            fullname += " ";

        fullname += ui->nameEdit->text();
    }

    if (!ui->patronymicEdit->text().isEmpty())
    {
        if (!fullname.isEmpty())
            fullname += " ";

        fullname += ui->patronymicEdit->text();
    }

    if (m_Table->seek(m_DataMapper->currentIndex()))
    {
        ui->fullnameEdit->setText(fullname);
        m_Table->setValue("t_fullname", fullname);
        //m_Table->update();
    }
}

void PersonPanel::inputPhone()
{
    EditContactDlg dlg(this);
    dlg.setReadOnly(ui->nameEdit->isReadOnly());
    if (dlg.exec() == QDialog::Accepted)
    {
        if (m_Contacts->newRec())
        {
            m_Contacts->setValue("t_personid", m_Table->value("t_id"));
            m_Contacts->setValue("t_type", TContact::TypePhone);
            m_Contacts->setValue("t_value", dlg.text());
            m_Contacts->insert();
        }
    }
}

void PersonPanel::inputEmail()
{
    EditEmailDlg dlg(this);
    dlg.setReadOnly(ui->nameEdit->isReadOnly());
    if (dlg.exec() == QDialog::Accepted)
    {
        if (m_Contacts->newRec())
        {
            m_Contacts->setValue("t_personid", m_Table->value("t_id"));
            m_Contacts->setValue("t_type", TContact::TypeEmail);
            m_Contacts->setValue("t_value", dlg.text());
            m_Contacts->insert();
        }
    }
}

void PersonPanel::inputAdress()
{
    EditAddressDlg dlg(this);
    dlg.setReadOnly(ui->nameEdit->isReadOnly());
    if (dlg.exec() == QDialog::Accepted)
    {
        if (m_Adress->newRec())
        {
            m_Adress->setValue("t_personid", m_Table->value("t_id"));
            m_Adress->setValue("t_address", dlg.text());
            m_Adress->insert();
        }
    }
}

void PersonPanel::editContact(const quint16 &row)
{
    if (m_Contacts->seek(row))
    {
        QString value = m_Contacts->value("t_value").toString();
        quint16 type = m_Contacts->value("t_type").value<quint16>();

        QScopedPointer<EditStringDialog> dlg;

        if (type == TContact::TypePhone)
            dlg.reset(new EditContactDlg(value, this));
        else
            dlg.reset(new EditEmailDlg(value, this));

        dlg->setReadOnly(ui->nameEdit->isReadOnly());
        if (dlg->exec() == QDialog::Accepted)
        {
            m_Contacts->setValue("t_value", dlg->text());
            m_Contacts->update();
        }
    }
}

void PersonPanel::editAdress(const quint16 &row)
{
    if (m_Adress->seek(row))
    {
        QString value = m_Adress->value("t_address").toString();

        QScopedPointer<EditStringDialog> dlg;
        dlg.reset(new EditAddressDlg(value, this));
        dlg->setReadOnly(ui->nameEdit->isReadOnly());

        if (dlg->exec() == QDialog::Accepted)
        {
            m_Adress->setValue("t_address", dlg->text());
            m_Adress->update();
        }
    }
}

void PersonPanel::contactsDoubleClicked(const QModelIndex &index)
{
    editContact(index.row());
}

void PersonPanel::adressDoubleClicked(const QModelIndex &index)
{
    editAdress(index.row());
}

void PersonPanel::adressContextMenu(const QPoint &pos)
{
    QAction *actionEdit = nullptr;
    QAction *actionRemove = nullptr;
    QAction *actionAdd = new QAction(QIcon::fromTheme("NewAddress"), tr("Добавить адрес"));
    actionAdd->setShortcut(m_AddAdressShortcut->key());
    //actions.append();

    QList<QAction*> actions = { actionAdd };

    QModelIndex index = ui->adressView->currentIndex();
    if (index.isValid())
    {
        if (m_Adress->seek(index.row()))
        {
            QAction *separator = new QAction();
            separator->setSeparator(true);
            actions.append(separator);

            QString adr = m_Adress->value("t_address").toString();
            QString value = adr;
            adr = adr.mid(0, 10);

            if (adr != value)
                value = adr + "...";

            QIcon actionEditIcon = QIcon::fromTheme("EditAddress");
            QIcon actionRemoveIcon = QIcon::fromTheme("RemoveAddress");

            actionEdit = new QAction(actionEditIcon, tr("Редактировать: %1").arg(value));
            actionRemove = new QAction(actionRemoveIcon, tr("Удалить: %1").arg(value));
            actionEdit->setData(index.row());
            actionRemove->setData(index.row());

            actions.append(actionEdit);
            actions.append(actionRemove);
        }
    }

    QAction *action = QMenu::exec(actions, ui->contactsView->mapToGlobal(pos), actions.first(), ui->contactsView);

    if (!action)
    {
        qDeleteAll(actions);
        return;
    }

    if (action == actionAdd)
        inputAdress();
    else if (action == actionEdit)
    {
        quint16 row = action->data().value<quint16>();
        editAdress(row);
    }
    else if (action == actionRemove)
    {
        QString value = m_Adress->value("t_address").toString();
        quint16 row = action->data().value<quint16>();

        QString adr = value;
        adr = adr.mid(0, 10);

        if (adr != value)
            value = adr + "...";

        QMessageBox::StandardButton button =
                QMessageBox::question(this, tr("Удаление"),
                                      tr("Удалить адрес \"<b>%1</b>\"?").arg(value),
                                      QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::Yes)
        {
            if (m_Adress->seek(row))
            {
                m_Adress->delete_();
            }
        }
    }

    qDeleteAll(actions);
}

void PersonPanel::contactsContextMenu(const QPoint &pos)
{
    enum ContactAction
    {
        ContactActionAddPhone = 0,
        ContactActionAddMail
    };

    QAction *actionEdit = nullptr;
    QAction *actionRemove = nullptr;

    QList<QAction*> actions =
    {
        new QAction(QIcon::fromTheme("NewPhone"), tr("Добавить телефон")),
        new QAction(QIcon::fromTheme("NewEmailAddress"), tr("Добавить электронный адрес"))
    };

    QModelIndex index = ui->contactsView->currentIndex();
    if (index.isValid())
    {
        if (m_Contacts->seek(index.row()))
        {
            QAction *separator = new QAction();
            separator->setSeparator(true);
            actions.append(separator);

            QString value = m_Contacts->value("t_value").toString();
            quint16 type = m_Contacts->value("t_type").value<quint16>();

            QIcon actionEditIcon = QIcon::fromTheme("EditPhone");
            QIcon actionRemoveIcon = QIcon::fromTheme("RemovePhone");
            if (type == TContact::TypeEmail)
            {
                actionEditIcon = QIcon::fromTheme("EditEmailAddress");
                actionRemoveIcon = QIcon::fromTheme("RemoveEmailAddress");
            }

            actionEdit = new QAction(actionEditIcon, tr("Редактировать: %1").arg(value));
            actionRemove = new QAction(actionRemoveIcon, tr("Удалить: %1").arg(value));
            actionEdit->setData(index.row());
            actionRemove->setData(index.row());

            actions.append(actionEdit);
            actions.append(actionRemove);
        }
    }

    actions[ContactActionAddPhone]->setShortcut(m_AddPhoneShortcut->key());
    actions[ContactActionAddMail]->setShortcut(m_AddEmailShortcut->key());

    QAction *action = QMenu::exec(actions, ui->contactsView->mapToGlobal(pos), actions.first(), ui->contactsView);

    if (!action)
    {
        qDeleteAll(actions);
        return;
    }

    if (action == actions[ContactActionAddPhone])
        inputPhone();
    else if (action == actions[ContactActionAddMail])
        inputEmail();
    else if (action == actionEdit)
    {
        quint16 row = action->data().value<quint16>();
        editContact(row);
    }
    else if (action == actionRemove)
    {
        QString value = m_Contacts->value("t_value").toString();
        quint16 row = action->data().value<quint16>();

        QMessageBox::StandardButton button =
                QMessageBox::question(this, tr("Удаление"),
                                      tr("Удалить контакт \"<b>%1</b>\"?").arg(value),
                                      QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::Yes)
        {
            if (m_Contacts->seek(row))
            {
                m_Contacts->delete_();
            }
        }
    }

    qDeleteAll(actions);
}

void PersonPanel::pumpOwns()
{
    qint16 personid = m_Table->value("t_id").value<qint16>();

    bool hr = m_PersonOwn->find(1, QVariantList() << personid);

    if (m_PersonOwn->rowCount())
    {
        for (; hr; hr = m_PersonOwn->next())
        {
            quint16 kind = m_PersonOwn->value("t_kind").value<quint16>();
            QListWidgetItem *item = ui->ownWidget->item(kind - 1);

            if (item)
                item->setCheckState(Qt::Checked);
        }
    }
}

void PersonPanel::ownChanged(QListWidgetItem *item)
{
    if (!item)
        return;

    int row = ui->ownWidget->row(item) + 1;

    if (item->checkState() == Qt::Checked)
    {
        quint16 personid = m_Table->value("t_id").value<quint16>();
        bool hr = m_PersonOwn->find(2, QVariantList() << personid << row);

        if (hr && m_PersonOwn->newRec())
        {
            m_PersonOwn->setValue("t_personid", personid);
            m_PersonOwn->setValue("t_kind", row);

            hr = m_PersonOwn->insert();
        }
    }
    else
    {
        quint16 personid = m_Table->value("t_id").value<quint16>();
        m_PersonOwn->delete_(2, QVariantList() << personid << row);
        /*bool hr = m_PersonOwn->find(2, QVariantList() << personid << row);

        if (hr)
        {
            hr = m_PersonOwn->delete_();
        }*/
    }
}

void PersonPanel::setViewMode()
{
    ui->buttonBox->setVisible(false);
    setReadOnly(m_AddPhoneShortcut);
    setReadOnly(m_AddEmailShortcut);
    setReadOnly(m_AddAdressShortcut);

    setReadOnly(ui->nameEdit);
    setReadOnly(ui->familyEdit);
    setReadOnly(ui->patronymicEdit);
    setReadOnly(ui->sexBox);

    setReadOnly(ui->contactsView);
    setReadOnly(ui->adressView);
    setReadOnly(ui->ownWidget);
    setReadOnly(ui->noteEdit);

    setWindowIcon(QIcon::fromTheme("Team"));
    setWindowTitle(tr("Просмотр субъекта: %1").arg(ui->fullnameEdit->text()));
}

void PersonPanel::setupJsEnbgine()
{
    JsClassExecuter executer;
    if (executer.open("scripts/personpanel.js"))
    {
        JsClassExecuter::setObjectOwner(m_Table.data());

        JsClassPtr obj = executer.createClass("PersonPanel", m_Table.data());
        std::optional<bool> ret = obj->call<bool>("checkSave");
        qDebug() << ret.value();
    }
}
