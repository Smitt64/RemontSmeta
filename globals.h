#ifndef GLOBALS_H
#define GLOBALS_H

#include <QObject>
#include <QSqlDatabase>
#include <QStringListModel>
#include <QDir>
#include "FactoryTemplate.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define KEY_OP |
#else
#define KEY_OP +
#endif

#if defined(QT_QML_DEBUG)
#include <QQmlDebuggingEnabler>
#endif

#define JSON_RENOVATION_TYPE "renovation_type"
#define JSON_PROMOTIONS "promotions"
#define JSON_BUILDTYPE "buildtype"
#define JSON_ROOMS "rooms"

#define JSON_NAMES "names"
#define JSON_MSC_STREETS "msc_streets"

class QSplashScreen;
class QStringListModel;
class QAbstractItemModel;
class SubWindowBase;
class JsonTableModel;
class JsConsole;
class DbTableFormat;
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

    bool hasJsonModel(const QString &name) const;
    JsonTableModel *model(const QString &name);

    const QDir &appdir() const;
    QFileInfo scriptFile(const QString &filename) const;

    QObject *jsConsoleObj();

    QSharedPointer<DbTableFormat> tableFormat(const QString &name) const;

private:
    Globals(QObject *parent = nullptr);
    void loadModels();
    void loadTables();
    void loadStreets();
    void loadNames();
    void openDataBase();
    void registerInterfaces();
    void initJsDebugging();

    bool loadStringListModelJson(const QString &fname, const QString &element, QStringListModel *model);

    static Globals *m_pThis;

    //QScopedPointer<QStringListModel> m_pStreetsModel, m_pNamesModel;

    QSqlDatabase m_DB;

    Factory<SubWindowBase, QString, QWidget*> m_WindowsFactory;
    QMap<QString,JsonTableModel*> m_JsonModels;
    QScopedPointer<QObject> m_JsConsole;

    QDir m_AppDir;
    QMap<QString,QSharedPointer<DbTableFormat>> m_TableFormats;

#if defined(QT_QML_DEBUG)
    QQmlDebuggingEnabler m_JsDebugger;
#endif
};

#endif // GLOBALS_H
