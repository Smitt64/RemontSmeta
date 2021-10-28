#include "jsconsole.h"
#include "loggingcategories.h"

JsConsole::JsConsole(QObject *parent) :
    QObject(parent)
{

}

void JsConsole::log(const QString& message)
{
    qInfo(logJsEngine()) << message;
}
