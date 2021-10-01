#include "globals.h"
#include "db/dbexception.h"
#include "loggingcategories.h"
#include <QSplashScreen>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringListModel>
#include <QFile>
#include <QtDebug>
#include <QStyleFactory>
#include <QIcon>

#include "tables/tclient.h"

Globals *Globals::m_pThis = nullptr;

Globals::Globals(QObject *parent) :
    QObject(parent)
{

}

Globals *Globals::inst()
{
    if (!m_pThis)
        m_pThis = new Globals();

    return m_pThis;
}

void Globals::init(QSplashScreen *screen)
{
    qCInfo(logCore()) << "Init";
    QStyle *style = QStyleFactory::create("fusion");
    qApp->setStyle(style);

    QIcon::setThemeSearchPaths(QStringList() << ":/icons/images");
    QIcon::setThemeName("dark-icons");
    qDebug() << QIcon::themeSearchPaths() << QIcon::themeName();

    screen->showMessage(tr("Загрузка справочника улиц..."), Qt::AlignLeft, Qt::white);
    QCoreApplication::processEvents();
    loadStreets();
    QCoreApplication::processEvents();

    screen->showMessage(tr("Открытие базы данных..."), Qt::AlignLeft, Qt::white);
    QCoreApplication::processEvents();
    openDataBase();
    QCoreApplication::processEvents();
}

void Globals::loadStreets()
{
    QFile f("msc_streets.json");

    qCInfo(logCore()) << "Loading street list";
    if (f.open(QIODevice::ReadOnly))
    {
        m_pStreetsModel = new QStringListModel(this);
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        QJsonArray streets = doc["streets"].toArray();

        QStringList lst;
        for (const QJsonValue &value : qAsConst(streets))
            lst.append(value.toString());

        m_pStreetsModel->setStringList(lst);
    }
    else
        throw ExceptionBase(tr("Не удалось загрузить список улиц"));
}

void Globals::openDataBase()
{
    m_DB = QSqlDatabase::addDatabase("QSQLITE");
    m_DB.setDatabaseName("./rdb.sqlite3");

    qCInfo(logCore()) << "Open database";
    if (!m_DB.open())
        throw DbException(m_DB.lastError());
}

QSqlDatabase &Globals::db()
{
    return m_DB;
}

QAbstractItemModel *Globals::streetsModel()
{
    return m_pStreetsModel;
}
