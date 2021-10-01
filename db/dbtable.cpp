#include "dbtable.h"
#include "loggingcategories.h"
#include "globals.h"
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QDate>
#include <QPointer>

bool ExecuteQuery(QSqlQuery *query, QString *err)
{

    QMap<QString, QVariant> values = query->boundValues();
    QMapIterator<QString, QVariant> i(values);
    while(i.hasNext())
    {
        i.next();
        qCInfo(logSql()) << i.key() << ": " << i.value();
    }

    bool result = query->exec();
    if (!result)
    {
        qCCritical(logSql()) << query->lastError().text();

        if (err != Q_NULLPTR)
            *err = query->lastError().text();
    }
    qCInfo(logSql()) << query->executedQuery();
    qCInfo(logSql()) << "Result:" << result;

    return result;
}

// ------------------------------------------------

int DbTableModelColumnProxy::columnCount(DbTable *table, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    table->m_SkipModelProxy = true;
    return table->columnCount(parent);
}

QVariant DbTableModelColumnProxy::headerData(DbTable *table, int section, Qt::Orientation orientation, int role) const
{
    table->m_SkipModelProxy = true;
    return table->headerData(section, orientation, role);
}

QVariant DbTableModelColumnProxy::data(DbTable *table, const QModelIndex &index, int role) const
{
    table->m_SkipModelProxy = true;
    return table->data(index, role);
}

bool DbTableModelColumnProxy::setData(DbTable *table, const QModelIndex &index, const QVariant &value, int role)
{
    table->m_SkipModelProxy = true;
    return table->setData(index, value, role);
}

// ------------------------------------------------

DbTableIndex::DbTableIndex(DbTable *parent, const QVector<quint16> &fldid, const bool &uniq, const bool &autoinc) :
    m_Parent(parent),
    m_AutoInc(autoinc),
    m_Uniq(uniq),
    m_Fld(fldid)
{
}

DbTableIndex::~DbTableIndex()
{
}

bool DbTableIndex::hasField(const quint16 &fld) const
{
    return m_Fld.contains(fld);
}

const bool &DbTableIndex::isAutoInc() const
{
    return m_AutoInc;
}

const bool &DbTableIndex::isUniq() const
{
    return m_Uniq;
}

const quint16 &DbTableIndex::field(const quint16 &id) const
{
    Q_ASSERT_X(id < m_Fld.size(), "DbTableIndex::field", "invalid field index");
    return m_Fld[id];
}

quint16 DbTableIndex::count() const
{
    return m_Fld.size();
}

// -------------------------------------------------------------------------

DbTable::DbTable(const QString &name, QSqlDatabase _db) :
    QAbstractTableModel(),
    m_Db(_db),
    m_PrimaryIndex(INVALID_PRIMARY_INDEX),
    m_Name(name),
    m_QuerySize(0),
    m_recPos(-1),
    m_pHandler(nullptr),
    m_ModelColumnProxy(nullptr),
    m_SkipModelProxy(false)
{
    m_pQuery.reset(new QSqlQuery(m_Db));
}

DbTable::~DbTable()
{
    m_Cache.clear();
}

void DbTable::fillFieldsNameIds()
{
    for (int i = 0; i < m_Fields.size(); i++)
        m_FieldsNameIds[m_Fields[i].name()] = i;
}

const QString &DbTable::name() const
{
    return m_Name;
}

const DbFieldBase &DbTable::field(const quint16 &id) const
{
    Q_ASSERT_X(id < m_Fields.size(), "DbTable::field", "invalid field index");
    return m_Fields[id];
}

const DbFieldBase &DbTable::field(const QString &id) const
{
    Q_ASSERT_X(m_FieldsNameIds.contains(id.toLower()), "DbTable::field", "field not exists");

    quint16 indx = m_FieldsNameIds[id.toLower()];
    return m_Fields[indx];
}

const DbTableIndex &DbTable::indexx(const quint16 &id) const
{
    Q_ASSERT_X(id < m_Indeces.size(), "DbTable::index", "invalid index number");
    return m_Indeces[id];
}

std::optional<DbTableIndex> DbTable::aincIndex() const
{
    for (const DbTableIndex &idx : m_Indeces)
    {
        if (idx.isAutoInc())
            return idx;
    }
    return {};
}

const qint16 &DbTable::primaryIndex() const
{
    return m_PrimaryIndex;
}

quint16 DbTable::fieldNum(const QString &name) const
{
    Q_ASSERT_X(m_FieldsNameIds.contains(name.toLower()), "DbTable::fieldNum", "field not exists");
    return m_FieldsNameIds[name.toLower()];
}

qint32 DbTable::countSelect(const QString &sql, const QVariantList &keys)
{
    qint32 size = -1;
    QSqlQuery query(m_Db);
    QString countSql = QString("select count(1) from (%1)").arg(sql);

    query.prepare(countSql);
    for (const QVariant &val : keys)
        query.addBindValue(val);

    bool result = ExecuteQuery(&query);

    if (result)
    {
        query.next();
        size = query.value(0).toInt();
    }

    return size;
}

bool DbTable::find(const DbTableIndex *index, const QVariantList &keys)
{
    bool result = true;
    m_QuerySize = 0;

    QString sql = selectSql(index);
    m_pQuery->prepare(sql);

    for (const QVariant &val : keys)
        m_pQuery->addBindValue(val);

    result = ExecuteQuery(m_pQuery.data());

    if (result)
    {
        m_recPos = QSql::BeforeFirstRow;
        first();
        resetCurrRec();
        m_QuerySize = countSelect(sql, keys);

        m_LastIndex.reset(new DbTableIndex(*index));
        m_m_LastIndexValues = keys;
    }

    return result;
}

bool DbTable::find(const quint16 &index, const QVariantList &keys)
{
    Q_ASSERT_X(index < m_Indeces.size(), "DbTable::find", "invalid index number");
    return find(&m_Indeces[index], keys);
}

bool DbTable::selectAll()
{
    bool result = true;
    m_QuerySize = 0;

    QString sql = selectSql();
    m_pQuery->prepare(sql);

    result = ExecuteQuery(m_pQuery.data());

    if (result)
    {
        m_recPos = QSql::BeforeFirstRow;
        first();
        resetCurrRec();
        m_QuerySize = countSelect(sql);
    }

    return result;
}

QVariant DbTable::value(const quint16 &id) const
{
    Q_ASSERT_X(id < m_Fields.size(), "DbTable::value", "invalid field index");
    Q_ASSERT_X(m_recPos != QSql::BeforeFirstRow && m_recPos != QSql::AfterLastRow, "DbTable::value", "invalid position");
    Q_ASSERT_X(m_Cache.contains(m_recPos), "DbTable::value", "invalid record");

    const DbFieldBase &fld = field(id);
    QSqlRecord *rec = m_Cache[m_recPos];

    if (fld.type() == QVariant::Date)
        return QDate::fromJulianDay(rec->value(id).value<qint64>());

    QVariant val = rec->value(id);
    return val;
}

QVariant DbTable::value(const QString &id) const
{
    Q_ASSERT_X(m_FieldsNameIds.contains(id.toLower()), "DbTable::value", "field not exists");
    return value(m_FieldsNameIds[id.toLower()]);
}

void DbTable::setValue(const quint16 &id, const QVariant &val)
{
    Q_ASSERT_X(id < m_Fields.size(), "DbTable::setValue", "invalid field index");
    Q_ASSERT_X(m_recPos != QSql::BeforeFirstRow && m_recPos != QSql::AfterLastRow, "DbTable::setValue", "invalid position");
    Q_ASSERT_X(m_Cache.contains(m_recPos), "DbTable::setValue", "invalid record");

    const DbFieldBase &fld = field(id);
    QSqlRecord *rec = m_Cache[m_recPos];

    QVariant newValue;
    if (fld.type() == QVariant::Date)
        newValue = val.toDate().toJulianDay();
    else
        newValue = val;

    if (newValue != rec->value(id))
    {
        rec->setValue(id, newValue);
        m_RecordActions.insert(m_recPos, RecordAction::ActionUpdate);
    }

    //emit dataChanged(index(m_recPos, 0), index(m_recPos, m_Fields.size()), {Qt::DisplayRole, Qt::EditRole});
}

void DbTable::setValue(const QString &id, const QVariant &val)
{
    Q_ASSERT_X(m_FieldsNameIds.contains(id.toLower()), "DbTable::value", "field not exists");
    setValue(m_FieldsNameIds[id.toLower()], val);
}

QVariant DbTable::operator[](const quint16 &id)
{
    return value(id);
}

QVariant DbTable::operator[](const QString &id)
{
    return value(id);
}

QString DbTable::whereSql(const DbTableIndex *index, const bool &range) const
{
    QString sql;
    QTextStream stream(&sql);

    if (index)
    {
        bool isFirst = true;
        stream << " where ";

        for (const quint16 &fldid : index->m_Fld)
        {
            const DbFieldBase &fld = field(fldid);

            if (!isFirst)
                stream << "and ";

            if (!range)
                stream << fld.name() << " = ? ";
            else
                stream << fld.name() << " between ? and ? ";

            isFirst = false;
        }
    }

    return sql;
}

QString DbTable::updateSql(const DbTableIndex *index) const
{
    QString sql;
    QTextStream stream(&sql);
    stream << "update " << m_Name << " set ";

    bool isFirst = true;
    for (const DbFieldBase &fld : m_Fields)
    {
        if (!isFirst)
            stream << ", ";

        stream << fld.name() << " = ?";
        isFirst = false;
    }

    stream << whereSql(index);

    return sql;
}

QString DbTable::selectSql(const DbTableIndex *index) const
{
    QString sql;
    QTextStream stream(&sql);

    bool isFirst = true;
    stream << "select ";

    for (const DbFieldBase &fld : m_Fields)
    {
        if (!isFirst)
            stream << ", ";

        stream << fld.name();
        isFirst = false;
    }

    stream << " from " << m_Name;
    stream << whereSql(index);

    return sql;
}

QString DbTable::deleteSql(const DbTableIndex *index) const
{
    QString sql;
    QTextStream stream(&sql);

    stream << "delete from " << m_Name << " " << whereSql(index);
    return sql;
}

QString DbTable::insertSql() const
{
    QString sql;
    QTextStream stream(&sql);

    bool isFirst = true;
    stream << "insert into " << m_Name << "(";
    for (const DbFieldBase &fld : m_Fields)
    {
        if (!isFirst)
            stream << ", ";

        stream << fld.name();
        isFirst = false;
    }

    isFirst = true;
    stream << ") values(";
    for (const DbFieldBase &fld : m_Fields)
    {
        Q_UNUSED(fld);
        if (!isFirst)
            stream << ", ";

        stream << "?";
        isFirst = false;
    }
    stream << ")";

    return sql;
}

bool DbTable::update()
{
    Q_ASSERT_X(m_recPos != QSql::BeforeFirstRow && m_recPos != QSql::AfterLastRow, "DbTable::update", "invalid position");
    Q_ASSERT_X(m_Cache.contains(m_recPos), "DbTable::update", "invalid record");
    Q_ASSERT_X(m_PrimaryIndex >= 0 && m_PrimaryIndex < m_Indeces.size(), "DbTable::update", "invalid primary index");
    bool hr = true;

    const DbTableIndex &idx = indexx(m_PrimaryIndex);

    QString sql = updateSql(&idx);
    QSqlRecord *rec = m_Cache[m_recPos];

    QSqlQuery query(m_Db);
    query.prepare(sql);

    for (int i = 0; i < m_Fields.size(); i++)
    {
        QVariant val = rec->value(i);

        if (val.isValid())
            query.addBindValue(val);
        else
        {
            qCInfo(logDbTable()) << PTR_TO_HEX(this) << ": Update. Invalid bind parameter: pos =" << m_recPos << "/ field =" << m_Fields[i].name();

            hr = false;
            break;
        }
    }

    for (const quint16 &index_fld : idx.m_Fld)
    {
        QVariant val = rec->value(index_fld);

        if (val.isValid())
            query.addBindValue(rec->value(index_fld));
        else
        {
            qCInfo(logDbTable()) << PTR_TO_HEX(this) << ": Update. Invalid bind index parameter: pos =" << m_recPos << "/ field =" << m_Fields[index_fld].name();

            hr = false;
            break;
        }
    }

    if (hr)
        hr = ExecuteQuery(&query);

    if (hr)
        m_RecordActions.remove(m_recPos);

    return hr;
}

bool DbTable::insert()
{
    Q_ASSERT_X(m_recPos != QSql::BeforeFirstRow && m_recPos != QSql::AfterLastRow, "DbTable::insert", "invalid position");
    Q_ASSERT_X(m_Cache.contains(m_recPos), "DbTable::insert", "invalid record");

    bool hr = true;
    QString sql = insertSql();
    QSqlRecord *rec = m_Cache[m_recPos];

    QSqlQuery query(m_Db);
    query.prepare(sql);

    for (int i = 0; i < m_Fields.size(); i++)
    {
        QVariant val = rec->value(i);

        if (val.isValid())
            query.addBindValue(val);
        else
        {
            std::optional<DbTableIndex> autoindex = aincIndex();

            if (!autoindex || i != (*autoindex).field(0))
            {
                qCInfo(logDbTable()) << PTR_TO_HEX(this) << ": Insert. Invalid bind parameter: pos =" << m_recPos << "/ field =" << m_Fields[i].name();
                hr = false;
                break;
            }
            else
                query.addBindValue(val);
        }
    }

    if (hr)
        hr = ExecuteQuery(&query);

    if (hr)
    {
        m_RecordActions.remove(m_recPos);
        std::optional<DbTableIndex> aincindex = aincIndex();

        if (aincindex)
        {
            const quint16 &fld = aincindex->field(0);

            qint16 id = rec->value(fld).value<qint16>();
            if (id == 0)
            {
                QVariant newId = lastInsertRowId();
                rec->setValue(fld, newId);
            }
        }
    }

    return hr;
}

bool DbTable::delete_()
{
    Q_ASSERT_X(m_recPos != QSql::BeforeFirstRow && m_recPos != QSql::AfterLastRow, "DbTable::delete_", "invalid position");
    Q_ASSERT_X(m_Cache.contains(m_recPos), "DbTable::delete_", "invalid record");

    beginResetModel();
    bool hr = true;
    const DbTableIndex &idx = indexx(primaryIndex());

    QSqlRecord *rec = m_Cache[m_recPos];
    QString sql = deleteSql(&idx);

    QSqlQuery query(m_Db);
    query.prepare(sql);

    quint16 index_size = idx.count();
    for (quint16 i = 0; i < index_size; i++)
        query.addBindValue(rec->value(i));

    hr = ExecuteQuery(&query);

    if (hr)
        hr = reset();
        //m_RecordActions.remove(m_recPos);
    endResetModel();

    return hr;
}

bool DbTable::delete_(const quint16 &index, const QVariantList &keys)
{
    Q_ASSERT_X(index < m_Indeces.size(), "DbTable::delete_", "invalid index number");
    bool hr = true;

    const DbTableIndex &idx = indexx(index);
    QString sql = deleteSql(&idx);

    QSqlQuery query(m_Db);
    query.prepare(sql);

    for (const QVariant &val : keys)
        query.addBindValue(val);

    hr = ExecuteQuery(&query);

    return hr;
}

void DbTable::initSqlRecord(QSqlRecord **rec)
{
    Q_ASSERT_X(rec, "DbTable::initSqlRecord", "null pointer");
    *rec = new QSqlRecord();

    for (const DbFieldBase &fld : qAsConst(m_Fields))
        (*rec)->append(QSqlField(fld.name(), fld.type()));
}

bool DbTable::newRecPrivate()
{
    bool hr = false;
    m_recPos = m_QuerySize++;

    QSqlRecord *rec = nullptr;
    initSqlRecord(&rec);
    hr = m_Cache.insert(m_recPos, rec);

    for (int i = 0; i < m_Fields.size(); i++)
    {
        const DbFieldBase &fld = m_Fields[i];
        switch(fld.type())
        {
        case QVariant::Int:
            m_Cache[m_recPos]->setValue(i, 0);
            break;
        case QVariant::Date:
            m_Cache[m_recPos]->setValue(i, 0);
            break;
        default:
            m_Cache[m_recPos]->setValue(i, "");
        }
    }

    if (m_pHandler)
        m_pHandler->onNewRecord(this);

    m_SaveCurRec.reset(new QSqlRecord(*m_Cache[m_recPos]));

    return hr;
}

bool DbTable::newRec()
{
    bool hr = false;
    beginInsertRows(QModelIndex(), m_QuerySize, m_QuerySize);
    hr = newRecPrivate();
    endInsertRows();
    return hr;
}

quint16 DbTable::lastInsertRowId()
{
    quint16 val = 0;
    QSqlQuery query(m_Db);
    query.prepare("SELECT last_insert_rowid()");

    bool hr = ExecuteQuery(&query);

    if (hr && query.next())
        val = query.value(0).value<quint16>();

    return val;
}

QSqlRecord &DbTable::record() const
{
    Q_ASSERT_X((m_recPos != QSql::BeforeFirstRow && m_recPos != QSql::AfterLastRow), "DbTable::record", "invalid position");
    Q_ASSERT_X(m_Cache.contains(m_recPos), "DbTable::record", "invalid record");

    return *m_Cache[m_recPos];
}

QSqlRecord DbTable::recordBefore() const
{
    Q_ASSERT_X(m_SaveCurRec, "DbTable::recordBefor", "invalid record");
    return *m_SaveCurRec;
}

void DbTable::setRecord(const QSqlRecord &rec)
{
    Q_ASSERT_X(m_recPos != QSql::BeforeFirstRow && m_recPos != QSql::AfterLastRow, "DbTable::setRecord", "invalid position");
    m_Cache.remove(m_recPos);

    m_Cache.insert(m_recPos, new QSqlRecord(rec));
    m_SaveCurRec.reset(new QSqlRecord(rec));

    m_RecordActions.insert(m_recPos, RecordAction::ActionUpdate);
}

void DbTable::setHandler(DbTableHandler *handler)
{
    m_pHandler = handler;
}

QVariant DbTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (m_ModelColumnProxy && !m_SkipModelProxy)
    {
        DbTable *pThis = const_cast<DbTable*>(this);
        return m_ModelColumnProxy->headerData(pThis, section, orientation, role);
    }

    QVariant value = QAbstractTableModel::headerData(section, orientation, role);

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section >= 0 && section < m_Fields.size())
            value = m_Fields[section].name();
    }

    return value;
}

int DbTable::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_QuerySize;
}

int DbTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (m_ModelColumnProxy && !m_SkipModelProxy)
    {
        DbTable *pThis = const_cast<DbTable*>(this);
        return m_ModelColumnProxy->columnCount(pThis, parent);
    }

    return m_Fields.size();
}

Qt::ItemFlags DbTable::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractTableModel::flags(index) | Qt::ItemIsDropEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled;
}

QVariant DbTable::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_QuerySize)
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::DecorationRole)
        return QVariant();

    QVariant val;
    DbTable *pThis = const_cast<DbTable*>(this);
    if (m_ModelColumnProxy && !m_SkipModelProxy)
    {
        if (pThis->seek(index.row()))
        {
            pThis->m_recPos = index.row();
            pThis->resetCurrRec();

            if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::DecorationRole)
                val = m_ModelColumnProxy->data(pThis, index, role);
        }
    }
    else
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            DbTable *pThis = const_cast<DbTable*>(this);
            if (pThis->seek(index.row()))
            {
                pThis->m_recPos = index.row();
                pThis->resetCurrRec();
                val = value(index.column());
            }
        }
    }
    return val;
}

void DbTable::sendDataChanged(const QModelIndex &index)
{
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
}

bool DbTable::setData(const QModelIndex &ind, const QVariant &value, int role)
{
    bool result = false;
    if (ind.row() >= 0 && ind.row() < m_QuerySize)
    {
        if (m_ModelColumnProxy && !m_SkipModelProxy)
        {
            if (m_Cache.contains(ind.row()))
            {
                m_recPos = ind.row();
                resetCurrRec();
                result = m_ModelColumnProxy->setData(this, ind, value, role);
            }
        }
        else
        {
            if (role == Qt::DisplayRole || role == Qt::EditRole)
            {
                if (seek(ind.row()))
                {
                    m_recPos = ind.row();
                    resetCurrRec();
                    setValue(ind.column(), value);
                    result = true;
                    emit dataChanged(ind, ind, {Qt::DisplayRole, Qt::EditRole});
                }
            }
        }
    }

    return result;
}

void DbTable::resetCurrRec()
{
    if (!m_Cache.contains(m_recPos))
    {
        //m_CurrRec.reset(new QSqlRecord(m_pQuery->record()));
        m_Cache.insert(m_recPos, new QSqlRecord(m_pQuery->record()));
        m_SaveCurRec.reset(new QSqlRecord(m_pQuery->record()));
    }
}

#define DBTABLE_CALL_FUNC(func, what) bool hr = m_pQuery->func(); if (hr) { what; resetCurrRec(); } return hr;

bool DbTable::next()
{
    DBTABLE_CALL_FUNC(next, ++m_recPos);
}

bool DbTable::previous()
{
    DBTABLE_CALL_FUNC(previous, --m_recPos);
}

bool DbTable::first()
{
    DBTABLE_CALL_FUNC(first, m_recPos = 0);
}

bool DbTable::last()
{
    DBTABLE_CALL_FUNC(last, m_recPos = m_QuerySize - 1);
}

bool DbTable::seek(const quint16 &index)
{
    bool hr = false;
    if (index >= m_QuerySize)
        hr = m_Cache.contains(index);
    else
    {
        hr = m_pQuery->seek(index);

        if (hr)
            resetCurrRec();
        else
            hr = m_Cache.contains(index);
    }

    if (hr)
    {
        m_recPos = index;
        resetCurrRec();
    }
    return hr;
}

void DbTable::setProxy(DbTableModelColumnProxy *proxy)
{
    m_ModelColumnProxy = proxy;
}

bool DbTable::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (count < 1 || row < 0 || row > rowCount(parent))
        return false;

    bool hr = true;
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; i++)
    {
        hr = newRec();

        if (!hr)
            break;
        else
            m_RecordActions.insert(m_recPos, RecordAction::ActionInsert);
    }
    endInsertRows();
    return hr;
}

bool DbTable::submit()
{
    qCInfo(logDbTable()) << PTR_TO_HEX(this) << ": Submit. Connection" << m_Db.connectionName();
    bool hr = m_Db.transaction();
    qCInfo(logSql()) << "Begin transaction:" << hr;

    ActionMap::key_type saveRecPos = m_recPos;
    QList<ActionMap::key_type> cachedKeys = m_RecordActions.keys();
    Globals::inst()->uniqueList(cachedKeys);

    for (const ActionMap::key_type &i : qAsConst(cachedKeys))
    {
        QList<RecordAction> recordActions = m_RecordActions.values(i);
        std::sort(recordActions.begin(), recordActions.end());
        Globals::inst()->uniqueList(recordActions);

        hr = seek(i);

        if (hr)
        {
            if (recordActions.contains(ActionInsert))
            {
                hr = insert();
                emit dataChanged(index(i, 0), index(i, m_Fields.size()));
            }
            else
                hr = update();
        }

        if (!hr)
            break;
    }

    if (hr)
    {
        if (cachedKeys.contains(saveRecPos))
            m_recPos = QSql::BeforeFirstRow;
        else
            m_recPos = saveRecPos;
    }
    else
        m_recPos = saveRecPos;

    if (hr)
    {
        hr = m_Db.commit();
        qCInfo(logSql()) << "Commit transaction:" << hr;
    }
    else
    {
        m_Db.rollback();
        qCInfo(logSql()) << "Rollback transaction";
    }

    qCInfo(logDbTable()) << PTR_TO_HEX(this) << ": Submit. Status:" << hr;

    return hr;
}

bool DbTable::reset()
{
    bool hr = false;
    if (m_LastIndex)
        hr = find(m_LastIndex.data(), m_m_LastIndexValues);
    else
        hr = selectAll();

    if (hr)
    {
        m_Cache.clear();
        m_RecordActions.clear();
    }

    return hr;
}
