#ifndef DBFIELD_HPP
#define DBFIELD_HPP

#include <type_traits>
#include <QVariant>
#include <QMetaEnum>

#if QT_VERSION >= 0x060000
#define QFieldType QMetaType::Type
#else
#define QFieldType QVariant::Type
#endif

class DbFieldBase : public QObject
{
    Q_OBJECT
    friend class DbTableFormat;
public:
    enum FieldType
    {
        String = 1,
        Autoinc,
        Date,
        Integer,
        Real,
        ByteArray
    };
    Q_ENUM(FieldType);

    DbFieldBase(const QString &name, const qint16 &size = 0);
    DbFieldBase(const DbFieldBase &other);

    const QString &name() const;
    const FieldType &type() const;
    bool setTypeFromString(const QString &name);
    QFieldType qType() const;
    const quint16 &size() const;
    bool isSizableType() const;

    QString sqlType() const;

    DbFieldBase &operator=(const DbFieldBase &right);

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
            t = FieldType::Integer;
        else if (std::is_same<Type, QDate>::value)
            t = FieldType::Date;
        else if (std::is_same<Type, float>::value)
            t = FieldType::Real;
        else if (std::is_same<Type, QByteArray>::value)
            t = FieldType::ByteArray;
        return t;
    }
};

#endif // DBFIELD_HPP
