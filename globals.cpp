#include "globals.h"
#include "db/dbexception.h"
#include "loggingcategories.h"
#include "clientswindow/viewpartywindow.h"
#include "reference/renovationtypeeditor.h"
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

#define CALL_INIT_FUNCTIONS(func, mes)  \
    screen->showMessage(mes, Qt::AlignLeft, Qt::white); \
    QCoreApplication::processEvents(); \
    func(); \
    QCoreApplication::processEvents()

void Globals::init(QSplashScreen *screen)
{
    qCInfo(logCore()) << "Init";
    QStyle *style = QStyleFactory::create("fusion");
    qApp->setStyle(style);

    QIcon::setThemeSearchPaths(QStringList() << ":/icons/images");
    QIcon::setThemeName("dark-icons");
    qDebug() << QIcon::themeSearchPaths() << QIcon::themeName();

    CALL_INIT_FUNCTIONS(loadStreets, tr("Загрузка справочника улиц..."));
    CALL_INIT_FUNCTIONS(loadNames, tr("Загрузка справочника имен..."));
    CALL_INIT_FUNCTIONS(openDataBase, tr("Открытие базы данных..."));
    CALL_INIT_FUNCTIONS(registerInterfaces, tr("Регистрация интерфейсов..."));
}

bool Globals::loadStringListModelJson(const QString &fname, const QString &element, QStringListModel *model)
{
    bool hr = true;

    QFile f(fname);
    if (f.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        QJsonArray elements = doc[element].toArray();

        QStringList lst;
        for (const QJsonValue &value : qAsConst(elements))
            lst.append(value.toString());

        model->setStringList(lst);
    }
    else
        hr = false;

    return hr;
}

void Globals::loadStreets()
{
    //QFile f("msc_streets.json");

    qCInfo(logCore()) << "Loading street list";
    m_pStreetsModel.reset(new QStringListModel());

    bool result = loadStringListModelJson("msc_streets.json", "streets", m_pStreetsModel.data());
    if (!result)
        throw ExceptionBase(tr("Не удалось загрузить список улиц"));
    /*if (f.open(QIODevice::ReadOnly))
    {
        m_pStreetsModel.reset(new QStringListModel());
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        QJsonArray streets = doc["streets"].toArray();

        QStringList lst;
        for (const QJsonValue &value : qAsConst(streets))
            lst.append(value.toString());

        m_pStreetsModel->setStringList(lst);
    }
    else
        throw ExceptionBase(tr("Не удалось загрузить список улиц"));*/
}

void Globals::loadNames()
{
    qCInfo(logCore()) << "Loading street list";
    m_pNamesModel.reset(new QStringListModel());

    bool result = loadStringListModelJson(":/tools/json/names.json", "names", m_pNamesModel.data());
    if (!result)
        throw ExceptionBase(tr("Не удалось загрузить список имен"));
    /*QFile f(":/tools/json/names.json");

    qCInfo(logCore()) << "Loading names list";
    if (f.open(QIODevice::ReadOnly))
    {
        m_pNamesModel.reset(new QStringListModel());
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        QJsonArray streets = doc["names"].toArray();

        QStringList lst;
        for (const QJsonValue &value : qAsConst(streets))
            lst.append(value.toString());

        m_pNamesModel->setStringList(lst);
    }
    else
        throw ExceptionBase(tr("Не удалось загрузить список имен"));*/
}

void Globals::openDataBase()
{
    m_DB = QSqlDatabase::addDatabase("QSQLITE");
    m_DB.setDatabaseName("./rdb.sqlite3");

    qCInfo(logCore()) << "Open database";
    if (!m_DB.open())
        throw DbException(m_DB.lastError());
}

void Globals::registerInterfaces()
{
    qCInfo(logCore()) << "Register interfaces";
    m_WindowsFactory.add<ViewPartyWindow>("application/all-person");
    m_WindowsFactory.add<ViewClientsWindow>("application/clients");
    m_WindowsFactory.add<RenovationTypeEditor>("application/renovationtype");
}

SubWindowBase *Globals::create(const QString &id, QWidget *widget) const
{
    return m_WindowsFactory.create(id, widget);
}

QSqlDatabase &Globals::db()
{
    return m_DB;
}

QAbstractItemModel *Globals::streetsModel()
{
    return m_pStreetsModel.data();
}

QAbstractItemModel *Globals::namesModel()
{
    return m_pNamesModel.data();
}
