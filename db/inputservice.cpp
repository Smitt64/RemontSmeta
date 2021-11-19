#include "inputservice.h"
#include "db/DbField.hpp"
#include <QtAlgorithms>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlDriver>

using namespace SqlTools;

bool createMemoryTable(DbTable *table, QSqlDatabase m_Db)
{
    QString sql;
    QTextStream stream(&sql);
    const QSqlDriver *driver = m_Db.driver();

    bool isFirst = true;
    stream << "create table " << driver->escapeIdentifier(table->name(), QSqlDriver::TableName) << "(" << Qt::endl;

    const DbTableIndex &primaryIndex = table->indexx(table->primaryIndex());

    for (int i = 0; i < table->columnCount(); i++)
    {
        const DbFieldBase &fld = table->field(i);

        if (!isFirst)
            stream << ", " << Qt::endl;

        stream << driver->escapeIdentifier(fld.name(), QSqlDriver::FieldName) << " " << fld.sqlType();

        if (fld.type() == DbFieldBase::String)
            stream << QString("(%1)").arg(fld.size());

        if (primaryIndex.hasField(i))
            stream << " primary key";

        isFirst = false;
    }

    stream << Qt::endl << ")";

    qCInfo(logInputService()) << "createMemoryTable" << QString("[%1]").arg(table->name()) << "in" << m_Db.connectionName();
    /*QSqlQuery query(m_Db);
    query.prepare(sql);
    return ExecuteQuery(&query);*/
    return ExecuteQuery(sql, m_Db);
}


InputServiceBase::InputServiceBase():
    m_Parent(nullptr),
    m_MainService(false),
    m_Index(-1)
{

}

InputServiceBase::~InputServiceBase()
{
    qDeleteAll(m_ChildServices);

    if (m_MainService)
        m_Db.close();
}

void InputServiceBase::addChildService(InputServiceBase *service)
{
    service->setParent(this);
    m_ChildServices.append(service);
}

QSqlDatabase InputServiceBase::db() const
{
    return m_Db;
}

QSharedPointer<DbTable> InputServiceBase::createTableObj()
{
    return QSharedPointer<DbTable>(nullptr);
}

void InputServiceBase::setParent(InputServiceBase *parent)
{
    m_Parent = parent;
    m_Db = parent->m_Db;

    QSharedPointer<DbTable> t = createTableObj();
    createMemoryTable(t.data(), m_Db);
}

void InputServiceBase::initHandler()
{
    m_pHandler.reset(new InputServiceDbHandler());
}

void InputServiceBase::setHandlerToTable(DbTable *table)
{
    if (m_pHandler)
        table->setHandler(m_pHandler.data());
}

QString InputServiceBase::generateConnsctionName()
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 12;

    QString randomString;
    for(int i=0; i<randomStringLength; ++i)
    {
        int index = QRandomGenerator::global()->generate() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

bool InputServiceBase::syncData(DbTable *parent)
{
    Q_UNUSED(parent);
    return false;
}

bool InputServiceBase::onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew)
{
    Q_UNUSED(parent);
    Q_UNUSED(recOld);
    Q_UNUSED(recNew);
    return false;
}

bool InputServiceBase::precache()
{
    return false;
}

bool InputServiceBase::deleteOldData()
{
    return false;
}

void InputServiceBase::setIndex(const quint16 &idx, const QVariantList &values)
{
    m_Index = idx;
    m_IndexValues = values;
}

bool InputServiceBase::start()
{
    qCInfo(logInputService()) << PTR_TO_HEX(this) << ": Start";

    bool hr = m_Db.transaction();
    qCInfo(logSql()) << "Begin transaction:" << hr;

    if (hr)
        hr = precache();

    if (hr)
    {
        for (InputServiceBase *service : qAsConst(m_ChildServices))
        {
            hr = service->precache();

            if (!hr)
                break;
        }
    }

    if (hr)
    {
        hr = m_Db.commit();
        qCInfo(logSql()) << "Commit transaction:" << hr;
    }
    else
    {
        m_Db.rollback();
        qCInfo(logSql()) << "Rollback transaction:" << hr;
    }

    return hr;
}

bool InputServiceBase::deletePrivate()
{
    bool hr = deleteOldData();

    if (hr)
    {
        for (InputServiceBase *service : qAsConst(m_ChildServices))
        {
            hr = service->deleteOldData();

            if (!hr)
                break;
        }
    }

    return hr;
}

bool InputServiceBase::delete_()
{
    QSqlDatabase mainDb = QSqlDatabase::database();
    qCInfo(logInputService()) << PTR_TO_HEX(this) << ": Delete data";
    bool hr = beginTransaction(mainDb);

    if (hr)
        hr = deletePrivate();

    if (hr)
        commitTransaction(mainDb);
    else
        rollbackTransaction(mainDb);

    return hr;
}

bool InputServiceBase::sync()
{
    QSqlDatabase mainDb = QSqlDatabase::database();
    qCInfo(logInputService()) << PTR_TO_HEX(this) << ": Sync";
    bool hr = beginTransaction(mainDb);

    hr = deletePrivate();

    if (hr)
        hr = syncData();

    if (hr)
        commitTransaction(mainDb);
    else
        rollbackTransaction(mainDb);

    return hr;
}
