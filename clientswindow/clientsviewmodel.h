#ifndef CLIENTSVIEWMODEL_H
#define CLIENTSVIEWMODEL_H

#include <QSqlQueryModel>

class QSqlQuery;
class QEnterEvent;
class ClientsViewModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    enum ClientsModelFilds
    {
        FldID = 0,
        FldFullName,
        FldSex,
        FldPhone,
        FldEmail,
        FldAdress,
        FldNote
    };
    typedef QList<quint16> PartyOwnsList;

    ClientsViewModel(QObject *parent = nullptr);
    virtual ~ClientsViewModel();

    void resetQuery();
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void setPartyOwns(const PartyOwnsList &list);

private:
    QString getContactSubquery();

    PartyOwnsList m_PartyOwnsList;

#if QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
    QScopedPointer<QSqlQuery> m_pQuery;
#endif
};

#endif // CLIENTSVIEWMODEL_H
