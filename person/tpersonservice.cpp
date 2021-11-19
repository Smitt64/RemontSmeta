#include "tpersonservice.h"
#include <QDebug>

TPersonService::TPersonService(const quint32 &PersonId) :
    InputService(),
    m_PersonId(PersonId)
{
    initMainService();
    setIndex(0, QVariantList() << PersonId);
    m_ContactService = new TContactService(PersonId);
    m_AdressService = new TAdressService(PersonId);
    m_OwnService = new TPersonOwnService(PersonId);

    addChildService(m_ContactService);
    addChildService(m_AdressService);
    addChildService(m_OwnService);
}

TPersonService::~TPersonService()
{

}

QSharedPointer<DbTable> TPersonService::contactsTable()
{
    return m_ContactService->createTableObj();
}

QSharedPointer<DbTable> TPersonService::adressTable()
{
    return m_AdressService->createTableObj();
}

QSharedPointer<DbTable> TPersonService::ownTable()
{
    return m_OwnService->createTableObj();
}

// ------------------------------------------------

TContactService::TContactService(const quint32 &PersonId) :
    InputService(),
    m_PersonId(PersonId)
{
    setIndex(1, QVariantList() << PersonId);
}

TContactService::~TContactService()
{

}

bool TContactService::onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew)
{
    bool hr = InputService<TContact>::onInsertRecord(parent, recOld, recNew);

    if (hr && parent)
        recNew->setValue("t_personid", parent->value("t_id"));

    return hr;
}

// ------------------------------------------------

TAdressService::TAdressService(const quint32 &PersonId) :
    InputService(),
    m_PersonId(PersonId)
{
    setIndex(1, QVariantList() << PersonId);
}

TAdressService::~TAdressService()
{

}

bool TAdressService::onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew)
{
    bool hr = InputService<TAdress>::onInsertRecord(parent, recOld, recNew);

    if (hr && parent)
        recNew->setValue("t_personid", parent->value(0));

    return hr;
}

// ------------------------------------------------

TPersonOwnService::TPersonOwnService(const quint32 &PersonId) :
    InputService(),
    m_PersonId(PersonId)
{
    setIndex(1, QVariantList() << PersonId);
}

TPersonOwnService::~TPersonOwnService()
{

}

bool TPersonOwnService::onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew)
{
    bool hr = InputService<TPersonOwn>::onInsertRecord(parent, recOld, recNew);

    if (hr && parent)
        recNew->setValue("t_personid", parent->value(0));

    return hr;
}
