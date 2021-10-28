#ifndef EXCEPTIONBASE_H
#define EXCEPTIONBASE_H

#include <QException>
#include <QDebug>

class ExceptionBase : public QException
{
public:
    ExceptionBase(const QString &whatStr = QString()) :
        m_What(whatStr)
    {
    }

    virtual ~ExceptionBase()
    {

    }

    const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT Q_DECL_OVERRIDE
    {
        return QLatin1String(m_What.toUtf8()).latin1();
    }

protected:
    QString m_What;
};

#endif // EXCEPTIONBASE_H
