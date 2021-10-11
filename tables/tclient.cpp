#include "loggingcategories.h"
#include "tclient.h"
#include "person/tpersonservice.h"
#include <QtCore>
#include <QIcon>

TPerson::TPerson(QSqlDatabase _db) :
    DbTable("dperson_dbt", _db)
{
    DB_BEGIN_DEFINE
        DB_ADD_FIELD(int,"t_id"),
        DB_ADD_SFIELD(QString,"t_family", 155),
        DB_ADD_SFIELD(QString,"t_name", 155),
        DB_ADD_SFIELD(QString,"t_patronymic", 155),
        DB_ADD_SFIELD(QString,"t_fullname", 465),
        DB_ADD_FIELD(int,"t_sex"),
        DB_ADD_SFIELD(QString,"t_note", 1000)
    DB_END_DEFINE;

    DB_BEGIN_INDEX
        DB_ADD_AUTOINC_INDEX({0})
    DB_END_INDEX;

    DB_SET_PRIMARY_INDEX(0);
}

TPerson::~TPerson()
{

}

QString TPerson::getSexName(const quint8 &sex)
{
    QString value;
    switch(sex)
    {
    case SexMale:
        value = "Мужчина";
        break;
    case SexFemale:
        value = "Женщина";
        break;
    default:
        value = "";
    }
    return value;
}

QString TPerson::getSexChar(const quint8 &sex)
{
    QString value;
    switch(sex)
    {
    case SexMale:
        value = QChar(0x2640);
        break;
    case SexFemale:
        value = QChar(0x2642);
        break;
    default:
        value = "";
    }
    return value;
}

QIcon TPerson::getSexIcon(const quint8 &sex)
{
    QIcon value;
    switch(sex)
    {
    case SexMale:
        value = QIcon::fromTheme("male");
        break;
    case SexFemale:
        value = QIcon::fromTheme("famele");
        break;
    default:
        value = QIcon();
    }
    return value;
}

bool TPerson::deletePerson(const quint16 &id)
{
    qCInfo(logDbTable()) << QString("Delete person %1").arg(id);
    bool hr = true;

    QScopedPointer<TPersonService> service(new TPersonService(id));
    hr = service->delete_();

    qCInfo(logDbTable()) <<"Delete person. Status:" << hr;
    return hr;
}

// ------------------------------------------------

TContact::TContact(QSqlDatabase _db) :
    DbTable("dcontact_dbt", _db)
{
    DB_BEGIN_DEFINE
        DB_ADD_FIELD(int,"t_id"),
        DB_ADD_FIELD(int,"t_personid"),
        DB_ADD_FIELD(int,"t_type"),
        DB_ADD_SFIELD(QString,"t_value", 255)
    DB_END_DEFINE;

    DB_BEGIN_INDEX
        DB_ADD_AUTOINC_INDEX({0}),
        DB_ADD_INDEX({1})
    DB_END_INDEX;

    DB_SET_PRIMARY_INDEX(0);
}

TContact::~TContact()
{

}

// ------------------------------------------------

TAdress::TAdress(QSqlDatabase _db) :
    DbTable("daddress_dbt", _db)
{
    DB_BEGIN_DEFINE
        DB_ADD_FIELD(int,"t_id"),
        DB_ADD_FIELD(int,"t_personid"),
        DB_ADD_SFIELD(QString,"t_address", 512)
    DB_END_DEFINE;

    DB_BEGIN_INDEX
        DB_ADD_AUTOINC_INDEX({0}),
        DB_ADD_INDEX({1})
    DB_END_INDEX;

    DB_SET_PRIMARY_INDEX(0);
}

TAdress::~TAdress()
{

}

// ------------------------------------------------

TPersonOwn::TPersonOwn(QSqlDatabase _db) :
    DbTable("dpersonown_dbt", _db)
{
    DB_BEGIN_DEFINE
        DB_ADD_FIELD(int,"t_id"),
        DB_ADD_FIELD(int,"t_personid"),
        DB_ADD_FIELD(int,"t_kind")
    DB_END_DEFINE;

    DB_BEGIN_INDEX
        DB_ADD_AUTOINC_INDEX({0}),
        DB_ADD_INDEX({1}),
        DB_ADD_UNIQ_INDEX(QVector<quint16>() << 1 << 2)
    DB_END_INDEX;

    DB_SET_PRIMARY_INDEX(0);
}

TPersonOwn::~TPersonOwn()
{

}
