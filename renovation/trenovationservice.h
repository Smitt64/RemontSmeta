#ifndef TRENOVATIONSERVICE_H
#define TRENOVATIONSERVICE_H

#include "db/inputservice.h"
#include "tables/renovation.h"

class TRoomService;
class TRenovationService : public InputService<TRenovation>
{
public:
    TRenovationService(const quint32 &RenovationID = 0);
    virtual ~TRenovationService();

    QSharedPointer<DbTable> roomsTable();

private:
    TRoomService *m_RoomService;
};

// ------------------------------------------------

class TRoomService : public InputService<TRoom>
{
public:
    TRoomService(const quint32 &RenovationID);
    virtual ~TRoomService();

    virtual bool onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew) Q_DECL_OVERRIDE;

private:
    quint32 RenovationID;
};

#endif // TRENOVATIONSERVICE_H
