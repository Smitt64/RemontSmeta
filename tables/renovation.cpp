#include "renovation.h"
#include "db/dbtableformat.hpp"

TRenovation::TRenovation(QSqlDatabase _db) :
    DbTable("drenovation_dbt", _db)
{
    /*DB_BEGIN_DEFINE
        DB_ADD_FIELD(int,"t_id"),
        DB_ADD_FIELD(int,"t_personid"),
        DB_ADD_FIELD(int,"t_type"),
        DB_ADD_FIELD(QDate,"t_dateprep"),
        DB_ADD_SFIELD(QString,"t_docnumber", 100),
        DB_ADD_FIELD(int,"t_housetype"),
        DB_ADD_FIELD(int,"t_stock")
    DB_END_DEFINE;

    DB_BEGIN_INDEX
        DB_ADD_AUTOINC_INDEX({0}),
        DB_ADD_INDEX({1})
    DB_END_INDEX;

    DB_SET_PRIMARY_INDEX(0);*/
}

TRenovation::~TRenovation()
{

}
