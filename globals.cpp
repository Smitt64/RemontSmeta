#include "globals.h"
#include "db/dbexception.h"
#include "loggingcategories.h"
#include "clientswindow/viewpartywindow.h"
#include "reference/jsonmodeleditor.h"
#include "tables/tclient.h"
#include "models/jsontablemodel.h"
#include "jsclasses/jsconsole.h"
#include <QSplashScreen>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringListModel>
#include <QFile>
#include <QtDebug>
#include <QStyleFactory>
#include <QIcon>

// -qmljsdebugger=port:3768,services:DebugMessages,QmlDebugger,V8Debugger,QmlInspector,DebugTranslation

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
    m_AppDir = qApp->applicationDirPath();
    qCInfo(logCore()) <<  "Application dir:" << m_AppDir.path();

    QStyle *style = QStyleFactory::create("fusion");
    qApp->setStyle(style);

    QIcon::setThemeSearchPaths(QStringList() << ":/icons/images");
    QIcon::setThemeName("dark-icons");
    qDebug() << QIcon::themeSearchPaths() << QIcon::themeName();

    /*CALL_INIT_FUNCTIONS(loadStreets, tr("Загрузка справочника улиц..."));
    CALL_INIT_FUNCTIONS(loadNames, tr("Загрузка справочника имен..."));*/
    CALL_INIT_FUNCTIONS(openDataBase, tr("Открытие базы данных..."));
    CALL_INIT_FUNCTIONS(loadModels, tr("Загрузка справочников..."));
    CALL_INIT_FUNCTIONS(loadTables, tr("Загрузка описаний таблиц..."));
    CALL_INIT_FUNCTIONS(registerInterfaces, tr("Регистрация интерфейсов..."));

    initJsDebugging();
}

void Globals::initJsDebugging()
{
    m_JsConsole.reset(new JsConsole());
#if defined(QT_QML_DEBUG)
    //qCInfo(logCore()) << "Init java script debugging";
    //m_JsDebugger = qQmlEnableDebuggingHelper();
    /*m_JsDebugger = qQmlEnableDebuggingHelper;
    qDebug() << QQmlDebuggingEnabler::debuggerServices();
    qDebug() << QQmlDebuggingEnabler::startTcpDebugServer(1111);*/
#else
    qCInfo(logCore()) << "Init java script debugging not supported";
#endif
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
    /*qCInfo(logCore()) << "Loading street list";
    m_JsonModels[JSON_MSC_STREETS] = new JsonTableModel();

    bool result = m_JsonModels[JSON_MSC_STREETS]->open("json/msc_streets.json");
    if (!result)
        throw ExceptionBase(tr("Не удалось загрузить список улиц"));*/

    /*m_pStreetsModel.reset(new QStringListModel());

    bool result = loadStringListModelJson("json/msc_streets.json", "streets", m_pStreetsModel.data());
    if (!result)
        throw ExceptionBase(tr("Не удалось загрузить список улиц"));*/
}

void Globals::loadNames()
{
    /*qCInfo(logCore()) << "Loading street list";
    m_JsonModels[JSON_NAMES] = new JsonTableModel();

    bool result = m_JsonModels[JSON_NAMES]->open("json/names.json");
    if (!result)
        throw ExceptionBase(tr("Не удалось загрузить список имен"));*/

    /*m_pNamesModel.reset(new QStringListModel());

    bool result = loadStringListModelJson(":/tools/json/names.json", "names", m_pNamesModel.data());
    if (!result)
        throw ExceptionBase(tr("Не удалось загрузить список имен"));*/
}

void Globals::loadModels()
{
    QDir models_dir = m_AppDir;

    if (!models_dir.cd("json"))
    {
        qCritical(logCore()) << "Settings directory [json] not found";
        throw ExceptionBase(tr("Не найден каталог с настройками"));
    }
    else
    {
        if (!models_dir.cd("models"))
        {
            qCritical(logCore()) << "Models directory not found";
            throw ExceptionBase(tr("Не найден каталог с моделями"));
        }
        else
        {
            QFileInfoList models = models_dir.entryInfoList({"*.json"}, QDir::Files);

            for (const QFileInfo &model : models)
            {
                JsonTableModel *tablemodel = new JsonTableModel();
                bool hr = tablemodel->open(model.absoluteFilePath(), false);
                qCInfo(logCore()) << QString("Model [%1]:").arg(model.baseName()) << hr;

                if (hr)
                    m_JsonModels[model.baseName()] = tablemodel;
                else
                    delete tablemodel;
            }
        }
    }
}

void Globals::loadTables()
{
    QDir models_dir = m_AppDir;

    if (!models_dir.cd("json"))
    {
        qCritical(logCore()) << "Settings directory [json] not found";
        throw ExceptionBase(tr("Не найден каталог с настройками"));
    }
    else
    {
        if (!models_dir.cd("tables"))
        {
            qCritical(logCore()) << "Tables directory not found";
            throw ExceptionBase(tr("Не найден каталог с таблицами"));
        }
        else
        {
            QFileInfoList models = models_dir.entryInfoList({"*.json"}, QDir::Files);

            for (const QFileInfo &model : models)
                DbTableFormat::loadFromFile(model.absoluteFilePath(), m_TableFormats);
        }
    }
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
    m_WindowsFactory.add<JsonTableModelEditor>("application/jsonmodel");
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
    return m_JsonModels[JSON_MSC_STREETS];
}

QAbstractItemModel *Globals::namesModel()
{
    return m_JsonModels[JSON_NAMES];
}

bool Globals::hasJsonModel(const QString &name) const
{
    return m_JsonModels.contains(name);
}

JsonTableModel *Globals::model(const QString &name)
{
    return m_JsonModels[name];
}

const QDir &Globals::appdir() const
{
    return m_AppDir;
}

QFileInfo Globals::scriptFile(const QString &filename) const
{
    QDir d = appdir();
    d.cd("scripts");

    return QFileInfo(d.absoluteFilePath(filename));
}

QObject *Globals::jsConsoleObj()
{
    return m_JsConsole.data();
}

QSharedPointer<DbTableFormat> Globals::tableFormat(const QString &name) const
{
    if (!m_TableFormats.contains(name))
        return QSharedPointer<DbTableFormat>();

    return m_TableFormats[name];
}
