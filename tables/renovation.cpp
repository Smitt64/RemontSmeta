#include "renovation.h"
#include "db/dbtableformat.hpp"

TRenovation::TRenovation(QSqlDatabase _db) :
    DbTable("drenovation_dbt", _db)
{
}

TRenovation::~TRenovation()
{

}

// ------------------------------------------------

TRoom::TRoom(QSqlDatabase _db) :
    DbTable("droom_dbt", _db)
{
}

TRoom::~TRoom()
{

}
