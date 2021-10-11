#ifndef DBFIELD_HPP
#define DBFIELD_HPP

#include <type_traits>
#include <QVariant>

#if QT_VERSION >= 0x060000
#define QFieldType QMetaType::Type
#else
#define QFieldType QVariant::Type
#endif

class DbFieldBase
{
public:
    enum FieldType
    {
        String = 1,
        Date,
        Int,
        Double,
        ByteArray
    };

    DbFieldBase(const QString &name, const qint16 &size = 0)
    {
        m_Name = name.toLower();
        m_Size = size;
    }

    const QString &name() const
    {
        return m_Name;
    }

    const FieldType &type() const
    {
        return m_Type;
    }

    QFieldType qType() const
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
        case FieldType::Int:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            t = QMetaType::LongLong;
#else
            t = QVariant::LongLong;
#endif
            break;
        case FieldType::Double:
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

    const quint16 &size() const
    {
        return m_Size;
    }

    QString sqlType() const
    {
        QString type;

        switch(m_Type)
        {
        case FieldType::String:
            type = "TEXT";
            break;
        case FieldType::Date:
            type = "INTEGER";
            break;
        case FieldType::Int:
            type = "INTEGER";
            break;
        case FieldType::Double:
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

protected:
    QString m_Name;
    quint16 m_Size;
    FieldType m_Type;
};

template<class Type>
class DbField : public DbFieldBase
{
public:
    DbField(const QString &name, const qint16 &size = 0) :
        DbFieldBase(name, size)
    {
        m_Type = defineType();
    }
private:
    FieldType defineType() const
    {
        FieldType t = FieldType::String;
        if (std::is_same<Type, int>::value)
            t = FieldType::Int;
        else if (std::is_same<Type, QDate>::value)
            t = FieldType::Date;
        else if (std::is_same<Type, float>::value)
            t = FieldType::Double;
        else if (std::is_same<Type, QByteArray>::value)
            t = FieldType::ByteArray;
        return t;
    }
};

#endif // DBFIELD_HPP
