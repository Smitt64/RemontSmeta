#ifndef TPERSONSERVICE_H
#define TPERSONSERVICE_H

#include "db/inputservice.h"
#include "tables/tclient.h"

class TContactService;
class TAdressService;
class TPersonOwnService;
class TPersonService : public InputService<TPerson>
{
public:
    TPersonService(const quint32 &PersonId);
    virtual ~TPersonService();

    QSharedPointer<DbTable> contactsTable();
    QSharedPointer<DbTable> adressTable();
    QSharedPointer<DbTable> ownTable();

private:
    quint32 m_PersonId;
    TContactService *m_ContactService;
    TAdressService *m_AdressService;
    TPersonOwnService *m_OwnService;
};

// ------------------------------------------------

class TContactService : public InputService<TContact>
{
public:
    TContactService(const quint32 &PersonId);
    virtual ~TContactService();

    virtual bool onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew) Q_DECL_OVERRIDE;

private:
    quint32 m_PersonId;
};

// ------------------------------------------------

class TAdressService : public InputService<TAdress>
{
public:
    TAdressService(const quint32 &PersonId);
    virtual ~TAdressService();

    virtual bool onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew) Q_DECL_OVERRIDE;

private:
    quint32 m_PersonId;
};

// ------------------------------------------------

class TPersonOwnService : public InputService<TPersonOwn>
{
public:
    TPersonOwnService(const quint32 &PersonId);
    virtual ~TPersonOwnService();

    virtual bool onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew) Q_DECL_OVERRIDE;

private:
    quint32 m_PersonId;
};

#endif // TPERSONSERVICE_H
