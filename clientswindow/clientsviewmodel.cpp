#include "clientsviewmodel.h"
#include "tables/tclient.h"
#include "db/dbtable.h"
#include <QTextStream>
#include <QSqlQuery>
#include <QIcon>

using namespace SqlTools;

ClientsViewModel::ClientsViewModel(QObject *parent) :
    QSqlQueryModel(parent)
{
}

ClientsViewModel::~ClientsViewModel()
{

}

void ClientsViewModel::setPartyOwns(const PartyOwnsList &list)
{
    m_PartyOwnsList = list;
}

void ClientsViewModel::resetQuery()
{
    QString sql;
    QTextStream stream(&sql);

    stream << "SELECT t_id, t_fullname, t_sex, ";
    stream << getContactSubquery() << " t_phone, ";
    stream << getContactSubquery() << " t_email, ";
    stream << "(select t_address from daddress_dbt tp where tp.t_personid = T.t_id group by tp.t_personid having min(tp.t_id) order by tp.t_id) t_address, ";
    stream << "t_note ";
    stream << "FROM dperson_dbt T where 1 = 1 ";

    if (!m_PartyOwnsList.isEmpty())
    {
        QString owns;
        for (int i = 0; i < m_PartyOwnsList.count(); i++)
        {
            if (i != 0)
                owns += ", ";
            owns += "?";
        }
        stream << QString("AND EXISTS (SELECT 1 FROM dpersonown_dbt T2 WHERE T2.t_personid = T.t_id AND T2.t_kind in (%1))").arg(owns);
    }
    stream << "ORDER BY t_fullname";

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QSqlQuery query = QSqlQuery();
    query.prepare(sql);
    query.addBindValue(TContact::TypePhone);
    query.addBindValue(TContact::TypeEmail);

    for (const auto &own : m_PartyOwnsList)
        query.addBindValue(own);

    if (ExecuteQuery(&query))
        setQuery(std::move(query));
#else
    m_pQuery.reset(new QSqlQuery());

    m_pQuery->prepare(sql);
    m_pQuery->addBindValue(TContact::TypePhone);
    m_pQuery->addBindValue(TContact::TypeEmail);

    for (const auto &own : m_PartyOwnsList)
        m_pQuery->addBindValue(own);

    if (ExecuteQuery(m_pQuery.data()))
        setQuery(*m_pQuery);
#endif
}

QString ClientsViewModel::getContactSubquery()
{
    QString sql = QString("(select t_value from dcontact_dbt tp where tp.t_personid = T.t_id and tp.t_type = ? group by tp.t_personid having min(tp.t_id) order by tp.t_id)");
    return sql;
}

QVariant ClientsViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QStringList heads
    {
        "",
        tr("Полное имя"),
        tr("Пол"),
        tr("Телефон"),
        tr("Почта"),
        tr("Адресс"),
        tr("Примечание")
    };

    if (orientation == Qt::Vertical)
        return QSqlQueryModel::headerData(section, orientation, role);

    if (role == Qt::DisplayRole)
    {
        Q_ASSERT_X(section >= 0 && section < heads.size(), "ClientsViewModel::headerData", "invalid header number");
        return heads[section];
    }
    return QVariant();
}

QVariant ClientsViewModel::data(const QModelIndex &item, int role) const
{
    QVariant value = QSqlQueryModel::data(item, role);
    if (item.column() != FldSex)
        return value;

    if (role == Qt::DecorationRole)
    {
        quint8 type = QSqlQueryModel::data(item, Qt::DisplayRole).value<quint8>();
        return TPerson::getSexIcon(type);
    }

    return QVariant();
}
