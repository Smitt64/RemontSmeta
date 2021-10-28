#ifndef DBTABLEFORMAT_H
#define DBTABLEFORMAT_H

#include "db/DbField.hpp"
#include <QObject>
#include <QSharedPointer>
#include <QHash>

class DbTableIndex : public QObject
{
    Q_OBJECT
    friend class DbTable;
public:
    DbTableIndex(/*DbTable *parent, */const QVector<quint16> &fldid, const bool &uniq = false, const bool &autoinc = false);
    DbTableIndex(const DbTableIndex &other);
    virtual ~DbTableIndex();

    bool hasField(const quint16 &fld) const;

    const bool &isAutoInc() const;
    const bool &isUniq() const;

    const quint16 &field(const quint16 &id) const;
    quint16 count() const;

    DbTableIndex &operator=(const DbTableIndex &right);

private:
    //DbTable *m_Parent;
    bool m_AutoInc, m_Uniq;
    QVector<quint16> m_Fld;
};

class DbTableFormat;
typedef QMap<QString,QSharedPointer<DbTableFormat>> DbTableFormatMap;

class QJsonObject;
class DbFieldBase;
class ExceptionBase;
class DbTableFormat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(qint16 primaryIndex READ primaryIndex)
public:
    static QSharedPointer<DbTableFormat> fromJsonObject(const QJsonObject &obj) noexcept(false);

    const QString &name() const;
    const qint16 &primaryIndex() const;

    static void loadFromFile(const QString &filename, DbTableFormatMap &container) noexcept(false);

    const QVector<DbFieldBase> &fields() const;
    const QHash<QString,quint16> &fieldsNameIds() const;
    const QVector<DbTableIndex> &indeces() const;

private:
    DbTableFormat();

    QString m_Name;
    qint16 m_PrimaryIndex;

    QVector<DbFieldBase> m_Fields;
    QHash<QString,quint16> m_FieldsNameIds;
    QVector<DbTableIndex> m_Indeces;
};

#endif // DBTABLEFORMAT_H
