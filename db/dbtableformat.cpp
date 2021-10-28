#include "db/dbtableformat.hpp"
#include "db/DbField.hpp"
#include "exceptionbase.h"
#include "loggingcategories.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QFileInfo>

DbTableIndex::DbTableIndex(/*DbTable *parent, */const QVector<quint16> &fldid, const bool &uniq, const bool &autoinc) :
    QObject(),
    //m_Parent(parent),
    m_AutoInc(autoinc),
    m_Uniq(uniq),
    m_Fld(fldid)
{
}

DbTableIndex::DbTableIndex(const DbTableIndex &other) :
    QObject(),
    m_AutoInc(other.m_AutoInc),
    m_Uniq(other.m_Uniq)
{
    std::copy(other.m_Fld.begin(), other.m_Fld.end(), std::back_inserter(m_Fld));
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

DbTableIndex &DbTableIndex::operator=(const DbTableIndex &right)
{
    if (this == &right)
        return *this;

    m_AutoInc = right.m_AutoInc;
    m_Uniq = right.m_Uniq;
    std::copy(right.m_Fld.begin(), right.m_Fld.end(), std::back_inserter(m_Fld));

    return *this;
}

// ------------------------------------------------

DbFieldBase::DbFieldBase(const QString &name, const qint16 &size) :
    QObject()
{
    m_Name = name.toLower();
    m_Size = size;
}

DbFieldBase::DbFieldBase(const DbFieldBase &other) :
    QObject()
{
    m_Name = other.m_Name;
    m_Size = other.m_Size;
    m_Type = other.m_Type;
}

const QString &DbFieldBase::name() const
{
    return m_Name;
}

const DbFieldBase::FieldType &DbFieldBase::type() const
{
    return m_Type;
}

bool DbFieldBase::setTypeFromString(const QString &name)
{
    bool result = true;
    const QMetaObject * const metaObj = metaObject();
    int fldenumid = metaObj->indexOfEnumerator("FieldType");

    QString normalName = name;//(name[0] = name[0].toUpper());
    normalName[0] = normalName[0].toUpper();
    QMetaEnum enumFld = metaObj->enumerator(fldenumid);

    int value = enumFld.keyToValue(normalName.toLocal8Bit().data(), &result);

    if (result)
        m_Type = static_cast<FieldType>(value);

    return result;
}

QFieldType DbFieldBase::qType() const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QFieldType t = QMetaType::UnknownType;
#else
    QFieldType t = QVariant::Invalid;
#endif

    switch(m_Type)
    {
    case FieldType::String:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        t = QMetaType::QString;
#else
        t = QVariant::String;
#endif
        break;
    case FieldType::Date:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        t = QMetaType::QDate;
#else
        t = QVariant::Date;
#endif
        break;
    case FieldType::Integer:
    case FieldType::Autoinc:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        t = QMetaType::LongLong;
#else
        t = QVariant::LongLong;
#endif
        break;
    case FieldType::Real:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        t = QMetaType::QMetaType::Double;
#else
        t = QVariant::QVariant::Double;
#endif
        break;
    case FieldType::ByteArray:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        t = QMetaType::QMetaType::QByteArray;
#else
        t = QVariant::QVariant::ByteArray;
#endif
        break;
    }

    return t;
}

const quint16 &DbFieldBase::size() const
{
    return m_Size;
}

bool DbFieldBase::isSizableType() const
{
    if (m_Type == DbFieldBase::String)
        return true;
    return false;
}

QString DbFieldBase::sqlType() const
{
    QString type;

    switch(m_Type)
    {
    case FieldType::String:
        type = "TEXT";
        break;
    case FieldType::Date:
    case FieldType::Integer:
    case FieldType::Autoinc:
        type = "INTEGER";
        break;
    case FieldType::Real:
        type = "REAL";
        break;
    case FieldType::ByteArray:
        type = "BLOB";
        break;
    default:
        type = "";
    }

    return type;
}

DbFieldBase &DbFieldBase::operator=(const DbFieldBase &right)
{
    if (this == &right)
        return *this;

    m_Name = right.m_Name;
    m_Size = right.m_Size;
    m_Type = right.m_Type;

    return *this;
}

// ------------------------------------------------

DbTableFormat::DbTableFormat() :
    QObject(),
    m_PrimaryIndex(-1)
{

}

QSharedPointer<DbTableFormat> DbTableFormat::fromJsonObject(const QJsonObject &obj) noexcept(false)
{
    QSharedPointer<DbTableFormat> pPtr(new DbTableFormat());

    if (!obj.contains("name") || !obj.contains("primaryindex") || !obj.contains("fields"))
        throw ExceptionBase(tr("Не найдены обязательные поля в описании таблицы"));

    pPtr->m_PrimaryIndex = obj["primaryindex"].toInt();
    pPtr->m_Name = obj["name"].toString();

    if (!obj["fields"].isArray() || obj["fields"].toArray().empty())
        throw ExceptionBase(tr("Описание таблицы [%1] не содержит полей").arg(pPtr->m_Name));

    QJsonArray array = obj["fields"].toArray();
    for (const auto &field : array)
    {
        QJsonObject objFld;
        if (!field.isObject() || (objFld = field.toObject()).isEmpty() || !objFld.contains("name") || !objFld.contains("type"))
            throw ExceptionBase(tr("Описание полей таблицы [%1] не корректно: не указано имя поля или тип").arg(pPtr->m_Name));

        DbFieldBase fld(objFld["name"].toString());
        QString type = objFld["type"].toString();

        bool result = fld.setTypeFromString(type);
        if (!result)
            throw ExceptionBase(tr("Описание полей таблицы [%1] не корректно: не верно указан тип поля [%2]").arg(pPtr->m_Name, fld.name()));

        if (fld.isSizableType())
        {
            if (!objFld.contains("size"))
                throw ExceptionBase(tr("Описание полей таблицы [%1] не корректно: не указан размер поля [%2]").arg(pPtr->m_Name, fld.name()));

            fld.m_Size = objFld["size"].toInt();
        }

        pPtr->m_FieldsNameIds.insert(fld.name(), pPtr->m_Fields.size());
        pPtr->m_Fields.append(fld);
    }

    if (!obj["indices"].isArray() || obj["indices"].toArray().empty())
        throw ExceptionBase(tr("Описание таблицы [%1] не содержит индексов").arg(pPtr->m_Name));

    QJsonArray indices = obj["indices"].toArray();
    for (const auto &index : indices)
    {
        QJsonObject objFld;
        objFld = index.toObject();

        if (!index.isObject() || (objFld = index.toObject()).isEmpty() || !objFld.contains("fields") || !objFld["fields"].isArray())
            throw ExceptionBase(tr("Описание индексов таблицы [%1] не корректно: не указан перечень полей").arg(pPtr->m_Name));

        bool uniq = false;
        QVector<quint16> fldids;
        QJsonArray fields = objFld["fields"].toArray();
        for (const auto &indexfield : fields)
        {
            if (!pPtr->m_FieldsNameIds.contains(indexfield.toString()))
                throw ExceptionBase(tr("Описание индексов таблицы [%1] не корректно: таблица не содержит поле [%2]").arg(pPtr->m_Name, indexfield.toString()));

            qint16 fldid = pPtr->m_FieldsNameIds[indexfield.toString()];
            fldids.append(fldid);
        }

        if (objFld.contains("uniq"))
            uniq = objFld["uniq"].toBool();

        bool autoinc = false;
        if (fldids.size() == 1)
        {
            if (pPtr->m_Fields[fldids.first()].type() == DbFieldBase::Autoinc)
                autoinc = true;
        }

        DbTableIndex idx(fldids, uniq, autoinc);
        pPtr->m_Indeces.append(idx);
    }

    return pPtr;
}

void DbTableFormat::loadFromFile(const QString &filename, DbTableFormatMap &container) noexcept(false)
{
    QFile file(filename);
    QFileInfo fi(filename);
    if (!file.open(QIODevice::ReadOnly))
        throw ExceptionBase(tr("Не удалось открыть файл [%1]").arg(fi.baseName()));

    QJsonParseError parseResult;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseResult);

    if (parseResult.error != QJsonParseError::NoError)
        throw ExceptionBase(tr("Файл [%1] имеет не верный формат: %2").arg(fi.baseName(), parseResult.errorString()));

    QJsonObject root = doc.object();
    if (!root.contains("tables") || !root["tables"].isArray())
        throw ExceptionBase(tr("Файл [%1] имеет не верный формат: нет списка таблиц").arg(fi.baseName(), parseResult.errorString()));

    QJsonArray tables = root["tables"].toArray();
    for (const auto &table : tables)
    {
        QSharedPointer<DbTableFormat> tblptr = DbTableFormat::fromJsonObject(table.toObject());

        if (tblptr)
            container.insert(tblptr->name(), tblptr);
    }

    file.close();
}

const QString &DbTableFormat::name() const
{
    return m_Name;
}

const qint16 &DbTableFormat::primaryIndex() const
{
    return m_PrimaryIndex;
}

const QVector<DbFieldBase> &DbTableFormat::fields() const
{
    return m_Fields;
}

const QHash<QString,quint16> &DbTableFormat::fieldsNameIds() const
{
    return m_FieldsNameIds;
}

const QVector<DbTableIndex> &DbTableFormat::indeces() const
{
    return m_Indeces;
}
