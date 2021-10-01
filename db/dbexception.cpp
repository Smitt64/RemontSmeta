#include "dbexception.h"

DbException::DbException(const QString &dbError) :
    ExceptionBase(dbError)
{

}

DbException::DbException(const QSqlError &dbError)
{
    m_What = dbError.text();
}
