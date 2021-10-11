#ifndef GLOBALS_H
#define GLOBALS_H

#include <QObject>
#include <QSqlDatabase>
#include <QStringListModel>
#include "FactoryTemplate.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define KEY_OP |
#else
#define KEY_OP +
#endif


class QSplashScreen;
class QStringListModel;
class QAbstractItemModel;
class SubWindowBase;
class Globals : public QObject
{
    Q_OBJECT
public:
    static Globals *inst();
    void init(QSplashScreen *screen);

    QSqlDatabase &db();

    template<class T> void uniqueList(QList<T> &list)
    {
        auto last = std::unique(list.begin(), list.end());
        list.erase(last, list.end());
    }

    template <class C>
    void addWindowType(const QString &id)
    {
        m_WindowsFactory.add<C>(id);
    }

    SubWindowBase *create(const QString &id, QWidget *widget) const;
    QAbstractItemModel *streetsModel();
    QAbstractItemModel *namesModel();

private:
    Globals(QObject *parent = nullptr);
    void loadStreets();
    void loadNames();
    void openDataBase();
    void registerInterfaces();

    bool loadStringListModelJson(const QString &fname, const QString &element, QStringListModel *model);

    static Globals *m_pThis;

    QScopedPointer<QStringListModel> m_pStreetsModel, m_pNamesModel;

    QSqlDatabase m_DB;

    Factory<SubWindowBase, QString, QWidget*> m_WindowsFactory;
};

#endif // GLOBALS_H
