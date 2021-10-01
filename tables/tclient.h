#ifndef TCLIENT_H
#define TCLIENT_H

#include "db/dbtable.h"
#include <QVariant>
#include <QSqlDatabase>

class TPerson : public DbTable
{
public:
    TPerson(QSqlDatabase _db = QSqlDatabase::database());
    virtual ~TPerson();
};

// ------------------------------------------------

class TContact : public DbTable
{
public:
    enum Type : quint16
    {
        TypePhone = 1,
        TypeEmail,
    };
    TContact(QSqlDatabase _db = QSqlDatabase::database());
    virtual ~TContact();
};

// ------------------------------------------------

class TAdress : public DbTable
{
public:
    TAdress(QSqlDatabase _db = QSqlDatabase::database());
    virtual ~TAdress();
};

// ------------------------------------------------

class TPersonOwn : public DbTable
{
public:
    enum PartyOwn : quint16
    {
        OwnClient = 1,
        OwnEmployee,
    };
    TPersonOwn(QSqlDatabase _db = QSqlDatabase::database());
    virtual ~TPersonOwn();
};

#endif // TCLIENT_H
