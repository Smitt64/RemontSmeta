#ifndef EXCEPTIONBASE_H
#define EXCEPTIONBASE_H

#include <QException>

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
        return m_What.toLocal8Bit().data();
    }

protected:
    QString m_What;
};

#endif // EXCEPTIONBASE_H
