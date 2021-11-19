#ifndef JSCLASSEXECUTER_H
#define JSCLASSEXECUTER_H

#include <QObject>
#include <QQmlEngine>
#include <QJsonValue>

#define OWNER_PROPERTY "Owner"

template<class T>
void addParam(T &element, QQmlEngine *JsEngine, QJSValueList &params)
{
    if constexpr (std::is_convertible<T, QObject*>::value)
    {
        QJSValue value = JsEngine->newQObject(element);
        QVariant ownerProperty = element->property(OWNER_PROPERTY);

        if (ownerProperty.isValid())
            QJSEngine::setObjectOwnership(element, (QQmlEngine::ObjectOwnership)ownerProperty.toInt());

        params.append(value);
    }
    else //*if constexpr (std::is_same<T,QVariant>::value)*/
    {
        QJSValue value = JsEngine->toScriptValue<T>(element);
        params.append(value);
    }
}

bool isErrorJsValue(const QJSValue &value, QStringList exceptionStackTrace = QStringList());

// -------------------------------------------------------------------------

class JsClassWrapper
{
    friend class JsClassExecuter;

public:
    virtual ~JsClassWrapper();

    template<class RetType, class ...PrmTypes>
    std::optional<RetType> call(const QString &name, PrmTypes... paramsT)
    {
        std::optional<RetType> result;
        QJSValue ret = callBase(name, paramsT...);

        QVariant proxy = ret.toVariant();
        result = proxy.value<RetType>();

        return result;
    }

    template<class ...PrmTypes>
    void call(const QString &name, PrmTypes... paramsT)
    {
        callBase(name, paramsT...);
    }

private:
    JsClassWrapper(const QJSValue &object, QQmlEngine *JsEngine);

    template<class ...PrmTypes>
    QJSValue callBase(const QString &name, PrmTypes... paramsT)
    {
        if (m_Obj->isUndefined() || name.isEmpty())
            return QJSValue();

        QJSValue function = m_Obj->property(name);

        QJSValueList params;
        auto tuple = std::make_tuple(paramsT...);
        std::apply([&](PrmTypes ...x)
        {
            (..., addParam(x, m_pJsEngine, params));
        }, tuple);

        QJSValue ret = function.callWithInstance(*m_Obj.data(), params);
        if (!isErrorJsValue(ret))
        {
            if (m_pJsEngine->hasError())
            {
                QJSValue error = m_pJsEngine->catchError();

                if (isErrorJsValue(error))
                    return error;
            }
        }
        return ret;
    }

    QQmlEngine *m_pJsEngine;
    QScopedPointer<QJSValue> m_Obj;
};

// -------------------------------------------------------------------------

typedef QSharedPointer<JsClassWrapper> JsClassPtr;
class JsClassExecuter : public QObject
{
    Q_OBJECT
    friend class JsClassWrapper;
public:
    enum Ownership
    {
        CppOwnership = QQmlEngine::CppOwnership,
        JavaScriptOwnership = QQmlEngine::JavaScriptOwnership
    };
    Q_ENUMS(Ownership);

    constexpr static const char *Owner = "Owner";

    JsClassExecuter(QObject *parent = nullptr);
    virtual ~JsClassExecuter();

    static void setObjectOwner(QObject *obj, const Ownership &owner = JsClassExecuter::CppOwnership);

    template<class ...PrmTypes>
    JsClassPtr createClass(const QString &name, PrmTypes... paramsT)
    {
        QJSValue prototype = m_JsEngine->globalObject().property(name);

        if (isErrorJsValue(prototype))
            return nullptr;

        QJSValueList params;
        auto tuple = std::make_tuple(paramsT...);
        std::apply([&](PrmTypes ...x)
        {
            (..., addParam(x, m_JsEngine.data(), params));
        }, tuple);

        QJSValue obj = prototype.callAsConstructor(params);
        if (isErrorJsValue(prototype))
            return nullptr;
        else
        {
            if (m_JsEngine->hasError())
            {
                QJSValue error = m_JsEngine->catchError();

                if (isErrorJsValue(error))
                    return nullptr;
            }
        }

        if (!isObject(obj, name, "CreateClass"))
            return nullptr;

        JsClassPtr pPtr(new JsClassWrapper(obj, m_JsEngine.data()));

        return pPtr;
    }

public slots:
    bool open(const QString &filename);

private:
    bool isErrorValue(const QJSValue &value, QStringList exceptionStackTrace = QStringList());
    bool isObject(const QJSValue &value, const QString &objName, const QString &method);
    QScopedPointer<QQmlEngine> m_JsEngine;
    QString m_FileName;
};


#endif // JSCLASSEXECUTER_H
