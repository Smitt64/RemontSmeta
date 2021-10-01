#include "tclient.h"
#include <QtCore>

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
