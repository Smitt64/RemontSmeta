#ifndef DBFIELD_HPP
#define DBFIELD_HPP

#include <type_traits>
#include <QVariant>

class DbFieldBase
{
public:
    DbFieldBase(const QString &name, const qint16 &size = 0)
    {
        m_Name = name.toLower();
        m_Size = size;
    }

    const QString &name() const
    {
        return m_Name;
    }

    const QVariant::Type &type() const
    {
        return m_Type;
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
        case QVariant::String:
            type = "TEXT";
            break;
        case QVariant::Date:
            type = "INTEGER";
            break;
        case QVariant::Int:
            type = "INTEGER";
            break;
        case QVariant::Double:
            type = "REAL";
            break;
        case QVariant::ByteArray:
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
    QVariant::Type m_Type;
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
    QVariant::Type defineType() const
    {
        QVariant::Type t = QVariant::String;
        if (std::is_same<Type, int>::value)
            t = QVariant::Int;
        else if (std::is_same<Type, QDate>::value)
            t = QVariant::Date;
        else if (std::is_same<Type, float>::value)
            t = QVariant::Double;
        else if (std::is_same<Type, QByteArray>::value)
            t = QVariant::ByteArray;
        return t;
    }
};

#endif // DBFIELD_HPP
