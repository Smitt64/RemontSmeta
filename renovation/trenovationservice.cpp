#include "trenovationservice.h"

TRenovationService::TRenovationService(const quint32 &RenovationID) :
    InputService()
{
    initMainService();
    setIndex(0, QVariantList() << RenovationID);

    m_RoomService = new TRoomService(RenovationID);

    addChildService(m_RoomService);
}

TRenovationService::~TRenovationService()
{

}

QSharedPointer<DbTable> TRenovationService::roomsTable()
{
    return m_RoomService->createTableObj();
}

// ------------------------------------------------

TRoomService::TRoomService(const quint32 &RenovationID) :
    InputService(),
    RenovationID(RenovationID)
{
    setIndex(1, QVariantList() << RenovationID);
}

TRoomService::~TRoomService()
{

}

bool TRoomService::onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew)
{
    bool hr = InputService<TRoom>::onInsertRecord(parent, recOld, recNew);

    if (hr && parent)
        recNew->setValue("t_renovation", parent->value("t_id"));

    return hr;
}
