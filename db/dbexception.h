#ifndef DBEXCEPTION_H
#define DBEXCEPTION_H

#include <exceptionbase.h>
#include <QSqlError>

class DbException : public ExceptionBase
{
public:
    DbException(const QString &dbError);
    DbException(const QSqlError &dbError);
};

#endif // DBEXCEPTION_H
