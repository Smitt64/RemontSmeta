#include "loggingcategories.h"

Q_LOGGING_CATEGORY(logEmpty,    "Empty")
Q_LOGGING_CATEGORY(logCore,     "Core")
Q_LOGGING_CATEGORY(logDbTable,  "DbTable")
Q_LOGGING_CATEGORY(logSql,      "Sql")
Q_LOGGING_CATEGORY(logJsEngine, "JsEngine")
Q_LOGGING_CATEGORY(logInputService, "InputService")

QStringList LoggingCategoryList()
{
    return QStringList()
            << "Core"
            << "Sql"
            << "DbTable"
            << "InputService";
}

const QLoggingCategory &LoggingCategory(const QString &name)
{
    if (name == "Core")
        return logCore();
    if (name == "Sql")
        return logSql();
    if (name == "InputService")
        return logInputService();
    if (name == "DbTable")
        return logDbTable();

    return logEmpty();
}
