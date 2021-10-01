#ifndef GLOBALS_H
#define GLOBALS_H

#include <QObject>
#include <QSqlDatabase>

class QSplashScreen;
class QStringListModel;
class QAbstractItemModel;
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

    QAbstractItemModel *streetsModel();

private:
    Globals(QObject *parent = nullptr);
    void loadStreets();
    void openDataBase();

    static Globals *m_pThis;

    QStringListModel *m_pStreetsModel;

    QSqlDatabase m_DB;
};

#endif // GLOBALS_H
