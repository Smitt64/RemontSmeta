#include "jsclassexecuter.h"
#include "globals.h"
#include "loggingcategories.h"
#include <QFile>
#include <QDir>

JsClassWrapper::JsClassWrapper(const QJSValue &object, QQmlEngine *JsEngine) :
    m_pJsEngine(JsEngine),
    m_Obj(new QJSValue(object))
{

}

JsClassWrapper::~JsClassWrapper()
{

}

// -------------------------------------------------------------------------

JsClassExecuter::JsClassExecuter(QObject *parent) :
    QObject(parent)
{
    m_JsEngine.reset(new QQmlEngine());
    m_JsEngine->installExtensions(QJSEngine::AllExtensions);
    m_JsEngine->globalObject().setProperty("console", m_JsEngine->toScriptValue(Globals::inst()->jsConsoleObj()));
}

JsClassExecuter::~JsClassExecuter()
{

}

bool JsClassExecuter::open(const QString &filename)
{
    bool hr = true;
    QDir filedir = filename;

    if (filedir.isAbsolute())
        m_FileName = filedir.path();
    else
        m_FileName = Globals::inst()->appdir().absoluteFilePath(filename);

    QFile f(filename);
    if ((hr = f.open(QIODevice::ReadOnly)))
    {
        QByteArray scriptdata = f.readAll();

        QStringList exceptionStackTrace;
        QJSValue context = m_JsEngine->evaluate(scriptdata, filename, 0, &exceptionStackTrace);
        hr = !isErrorValue(context, exceptionStackTrace);
    }

    return hr;
}

bool JsClassExecuter::isErrorValue(const QJSValue &value, QStringList exceptionStackTrace)
{
    bool isError = value.isError();

    if (isError)
    {
        if (!exceptionStackTrace.empty())
        {
            qInfo(logJsEngine()) << "Stack trace:";
            for (const auto &line : exceptionStackTrace)
                qInfo(logJsEngine()) << line;
        }

        qInfo(logJsEngine()) << "Uncaught exception at line"
                             << value.property("lineNumber").toInt()
                             << ":" << value.toString();
    }

    return isError;
}

void JsClassExecuter::setObjectOwner(QObject *obj, const JsClassExecuter::Ownership &owner)
{
    QJSEngine::ObjectOwnership jsowner = static_cast<QJSEngine::ObjectOwnership>(owner);
    QJSEngine::setObjectOwnership(obj, jsowner);
}

bool JsClassExecuter::isObject(const QJSValue &value, const QString &objName, const QString &method)
{
    bool isobj = value.isObject();

    if (!isobj)
        qInfo(logJsEngine()) << QString("%1: can't create class, property [%2] is not object").arg(method, objName);

    return isobj;
}

/*QObject *JsClassExecuter::createClass(const QString &name, const QVariantList &params)
{

}*/
