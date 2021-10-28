#ifndef JSCONSOLE_H
#define JSCONSOLE_H

#include <QObject>

class JsConsole : public QObject
{
    Q_OBJECT
public:
    JsConsole(QObject *parent = nullptr);

    Q_INVOKABLE void log(const QString& message);
};

#endif // JSCONSOLE_H
