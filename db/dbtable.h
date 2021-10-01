#ifndef DBTABLE_H
#define DBTABLE_H

#include "DbField.hpp"
#include <QVector>
#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <optional>
#include <QCache>

#define INVALID_PRIMARY_INDEX -1

bool ExecuteQuery(QSqlQuery *query, QString *err = nullptr);

class DbTable;
class DbTableIndex
{
    friend class DbTable;
public:
    DbTableIndex(DbTable *parent, const QVector<quint16> &fldid, const bool &uniq = false, const bool &autoinc = false);
    virtual ~DbTableIndex();

    bool hasField(const quint16 &fld) const;

    const bool &isAutoInc() const;
    const bool &isUniq() const;

    const quint16 &field(const quint16 &id) const;
    quint16 count() const;


private:
    DbTable *m_Parent;
    bool m_AutoInc, m_Uniq;
    QVector<quint16> m_Fld;
};

// ------------------------------------------------

class DbTableHandler
{
public:
    virtual ~DbTableHandler() {}
    virtual void onNewRecord(DbTable *) {};
};

// ------------------------------------------------

class DbTableModelColumnProxy
{
public:
    virtual ~DbTableModelColumnProxy() {}

    virtual int columnCount(DbTable *table, const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant headerData(DbTable *table, int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual QVariant data(DbTable *table, const QModelIndex &index, int role) const;
    virtual bool setData(DbTable *table, const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
};

// ------------------------------------------------

class QSqlQuery;
class QSqlRecord;
class DbTable : public QAbstractTableModel
{
    Q_OBJECT
    friend class DbTableModelColumnProxy;
public:
    DbTable(const QString &name, QSqlDatabase _db = QSqlDatabase::database());
    virtual ~DbTable();

    const QString &name() const;
    const DbFieldBase &field(const quint16 &id) const;
    const DbFieldBase &field(const QString &id) const;
    const DbTableIndex &indexx(const quint16 &id) const;
    const qint16 &primaryIndex() const;
    std::optional<DbTableIndex> aincIndex() const;
    quint16 fieldNum(const QString &name) const;

    bool find(const quint16 &index, const QVariantList &keys);
    bool find(const DbTableIndex *index, const QVariantList &keys);
    bool selectAll();

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    QVariant value(const quint16 &id) const;
    QVariant value(const QString &id) const;

    void setValue(const quint16 &id, const QVariant &val);
    void setValue(const QString &id, const QVariant &val);

    QVariant operator[](const quint16 &id);
    QVariant operator[](const QString &id);

    QSqlRecord &record() const;
    QSqlRecord recordBefore() const;
    void setRecord(const QSqlRecord &rec);

    bool next();
    bool previous();
    bool first();
    bool last();
    bool seek(const quint16 &index);

    bool newRec();
    bool update();
    bool insert();
    bool delete_();
    bool delete_(const quint16 &index, const QVariantList &keys);

    void setHandler(DbTableHandler *handler);
    void setProxy(DbTableModelColumnProxy *proxy);

    quint16 lastInsertRowId();

    virtual bool submit() Q_DECL_OVERRIDE;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    void sendDataChanged(const QModelIndex &index);

protected:
    void fillFieldsNameIds();
    bool newRecPrivate();
    QString updateSql(const DbTableIndex *index = nullptr) const;
    QString selectSql(const DbTableIndex *index = nullptr) const;
    QString deleteSql(const DbTableIndex *index = nullptr) const;
    QString insertSql() const;
    QString whereSql(const DbTableIndex *index = nullptr, const bool &range = false) const;

    QVector<DbFieldBase> m_Fields;
    QHash<QString,quint16> m_FieldsNameIds;
    QVector<DbTableIndex> m_Indeces;
    QSqlDatabase m_Db;

    qint16 m_PrimaryIndex;

private:
    enum RecordAction : quint8
    {
        ActionInsert,
        ActionDelete,
        ActionUpdate
    };
    typedef QMultiMap<quint16, RecordAction> ActionMap;
    //typedef QPair<ActionMap::key_type, ActionMap::mapped_type> ActionMapPair;

    void resetCurrRec();
    void initSqlRecord(QSqlRecord **rec);
    bool reset();

    qint32 countSelect(const QString &sql, const QVariantList &keys = QVariantList());
    QString m_Name;
    qint32 m_QuerySize;
    QScopedPointer<QSqlQuery> m_pQuery;
    QScopedPointer<QSqlRecord> m_SaveCurRec;
    QCache<quint16,QSqlRecord> m_Cache{5000};
    ActionMap m_RecordActions;
    qint32 m_recPos;

    DbTableHandler *m_pHandler;
    DbTableModelColumnProxy *m_ModelColumnProxy;

    QScopedPointer<DbTableIndex> m_LastIndex;
    QVariantList m_m_LastIndexValues;

    bool m_SkipModelProxy;
};

#define DB_BEGIN_DEFINE m_Fields = {
#define DB_ADD_FIELD(type, name) DbField<type>(name)
#define DB_ADD_SFIELD(type, name, size) DbField<type>(name, size)
#define DB_END_DEFINE }; fillFieldsNameIds()

#define DB_BEGIN_INDEX m_Indeces = {
#define DB_ADD_INDEX(values) DbTableIndex(this, values)
#define DB_ADD_UNIQ_INDEX(values) DbTableIndex(this, values, true)
#define DB_ADD_AUTOINC_INDEX(values) DbTableIndex(this, values, true, true)
#define DB_END_INDEX }
#define DB_SET_PRIMARY_INDEX(index) m_PrimaryIndex = index

#endif // DBTABLE_H
