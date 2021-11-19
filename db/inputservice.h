#ifndef INPUTSERVICE_H
#define INPUTSERVICE_H

#include "db/dbtable.h"
#include "db/dbtableformat.hpp"
#include "loggingcategories.h"
#include <QSqlDatabase>
#include <QList>
#include <QSqlRecord>
#include <QSqlError>
#include <QRandomGenerator>
#include <QSharedPointer>

bool createMemoryTable(DbTable *table, QSqlDatabase m_Db);

class InputServiceBase
{
public:
    class InputServiceDbHandler : public DbTableHandler
    {
        qint32 m_AincValue = 0;
    public:
        virtual ~InputServiceDbHandler() {}
        void onNewRecord(DbTable *table)
        {
            std::optional<DbTableIndex> index = table->aincIndex();

            if (index)
            {
                --m_AincValue;
                table->setValue(index->field(0), m_AincValue);
            }
        }
    };

    InputServiceBase();
    virtual ~InputServiceBase();
    void addChildService(InputServiceBase *service);
    void setHandlerToTable(DbTable *table);

    bool start();
    bool sync();
    bool delete_();

    virtual QSharedPointer<DbTable> createTableObj();

    QSqlDatabase db() const;

    virtual bool onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew);
    virtual bool syncData(DbTable *parent = nullptr);

protected:
    virtual void initHandler();
    void setParent(InputServiceBase *parent);
    void setIndex(const quint16 &idx, const QVariantList &values);
    static QString generateConnsctionName();

    virtual bool deleteOldData();
    virtual bool precache();

    QList<InputServiceBase*> m_ChildServices;
    InputServiceBase *m_Parent;
    QSqlDatabase m_Db;
    QScopedPointer<InputServiceDbHandler> m_pHandler;
    bool m_MainService;

    quint16 m_Index;
    QVariantList m_IndexValues;

private:
    bool deletePrivate();
};

template<class TTable>
class InputService : public InputServiceBase
{
public:
    InputService() :
        InputServiceBase()
    {
        initHandler();
    }

    virtual ~InputService()
    {
    }

    virtual QSharedPointer<DbTable> createTableObj() Q_DECL_OVERRIDE
    {
        QSharedPointer<DbTable> ptr(new TTable(db()));
        setHandlerToTable(ptr.data());
        return ptr;
    }

protected:
    bool initMainService()
    {
        bool hr = true;
        qCInfo(logInputService()) << PTR_TO_HEX(this) << ": initMainService";
        m_Db = QSqlDatabase::addDatabase("QSQLITE" , generateConnsctionName());
        m_Db.setDatabaseName(":memory:");
        hr = m_Db.open();

        if (hr)
        {
            qCInfo(logInputService()) << PTR_TO_HEX(this) << ":" << m_Db.connectionName() << "memory database ok";
            QScopedPointer<TTable> t(new TTable());
            hr = createMemoryTable(t.data(), m_Db);
            m_MainService = true;
        }
        else
            qCInfo(logInputService()) << PTR_TO_HEX(this) << ":" << m_Db.connectionName() << "memory database" << m_Db.lastError().text();

        return hr;
    }

    bool copyTable(const quint16 &index, const QVariantList &keys)
    {
        bool hr = true;
        QScopedPointer<DbTable> pSrc(new TTable());
        hr = pSrc->find(index, keys);

        if (hr && pSrc->rowCount())
        {
            int i = 0;
            do {
                qCInfo(logInputService()) << PTR_TO_HEX(this) << ": copy rec to" << m_Db.connectionName();
                QScopedPointer<DbTable> pDst(new TTable(m_Db));
                QSqlRecord rec = pSrc->record();
                pDst->newRec();
                pDst->setRecord(rec);
                hr = pDst->insert();
                pSrc->next();
                ++i;

                if (!hr)
                    break;
            } while (i < pSrc->rowCount());
        }
        return hr;
    }

    virtual bool precache() Q_DECL_OVERRIDE
    {
        bool hr = false;

        if (m_Index >= 0)
            hr = copyTable(m_Index, m_IndexValues);

        return hr;
    }

    virtual bool deleteOldData() Q_DECL_OVERRIDE
    {
        QSqlDatabase mainDb = QSqlDatabase::database();
        bool hr = false;

        QScopedPointer<DbTable> table(new TTable(mainDb));
        hr = table->delete_(m_Index, m_IndexValues);

        return hr;
    }

    virtual bool onInsertRecord(DbTable *parent, QSqlRecord *recOld, QSqlRecord *recNew) Q_DECL_OVERRIDE
    {
        Q_UNUSED(parent);
        Q_UNUSED(recOld);
        QSharedPointer<DbTable> memTable = createTableObj();
        std::optional<DbTableIndex> aincindex = memTable->aincIndex();

        if (aincindex)
        {
            const quint16 &fld = aincindex->field(0);

            if (recNew->value(fld).value<qint16>() <= 0)
                recNew->setValue(fld, QVariant());
        }

        return true;
    }

    virtual bool syncData(DbTable *parent = nullptr) Q_DECL_OVERRIDE
    {
        QSqlDatabase mainDb = QSqlDatabase::database();
        bool hr = false;

        QSharedPointer<DbTable> memTable = createTableObj();
        hr = memTable->selectAll();

        if (hr && memTable->rowCount())
        {
            int i = 0;
            do {
                QScopedPointer<DbTable> maintable(new TTable(mainDb));
                QSqlRecord recOld = memTable->record();
                QSqlRecord recNew = memTable->record();

                hr = onInsertRecord(parent, &recOld, &recNew);

                if (hr)
                {
                    maintable->newRec();
                    maintable->setRecord(recNew);
                    hr = maintable->insert();
                }

                if (hr)
                {
                    for (InputServiceBase *service : m_ChildServices)
                    {
                        hr = service->syncData(maintable.data());

                        if (!hr)
                            break;
                    }
                }

                if (!hr)
                    break;

                memTable->next();
                ++i;
            } while (i < memTable->rowCount());
        }

        return hr;
    }
};

#endif // INPUTSERVICE_H
