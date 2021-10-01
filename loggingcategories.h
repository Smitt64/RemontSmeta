#ifndef LOGGINGCATEGORIES_H
#define LOGGINGCATEGORIES_H

#include <QString>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logEmpty)
Q_DECLARE_LOGGING_CATEGORY(logCore)
Q_DECLARE_LOGGING_CATEGORY(logDbTable)
Q_DECLARE_LOGGING_CATEGORY(logSql)
Q_DECLARE_LOGGING_CATEGORY(logInputService)

QStringList LoggingCategoryList();
const QLoggingCategory &LoggingCategory(const QString &name);

#define PTR_TO_HEX(ptr) (QString("0x%1").arg((quintptr)ptr, QT_POINTER_SIZE * 2, 16, QChar('0')))

#endif // LOGGINGCATEGORIES_H
