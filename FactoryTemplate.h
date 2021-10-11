#ifndef FACTORYTEMPLATE_H
#define FACTORYTEMPLATE_H

#include <QString>
#include <QHash>
#include <QMap>

template <class Base, class ...Types>
class AbstractCreator
{
public:
    AbstractCreator(const QString &pAlias) { m_alias = pAlias; }
    virtual ~AbstractCreator() { }
    virtual Base *create(Types... args) = 0;
    const QString alias() const { return m_alias; }

private:
    QString m_alias;
};

template <class C, class Base, class ...Types>
class Creator : public AbstractCreator<Base, Types...>
{
public:
    Creator(const QString &alias) :
        AbstractCreator<Base, Types...>(alias)
    { }
    virtual ~Creator() { }
    Base *create(Types... args) { return static_cast<Base*>(new C(args...)); }
};

template <class Base, class IdType, class ...Types>
class Factory
{
protected:
    typedef AbstractCreator<Base, Types...> AbstractFactory;
    typedef QMap<IdType, AbstractFactory*> FactoryMap;

public:
    Factory() { }
    virtual ~Factory() { }

    template <class C>
    void add(const IdType &id, const QString &alias = QString())
    {
        registerClass(id, new Creator<C, Base, Types...>(alias));
    }

    Base *create(const IdType &id, Types... args) const
    {
        typename FactoryMap::const_iterator it = _LibFactory.find(id);

        if (it == _LibFactory.end())
            return Q_NULLPTR;
        return it.value()->create(args...);
    }

    Base *getInstance(const IdType &id, Types... args) const
    {
        typename FactoryMap::const_iterator it = _LibFactory.find(id);
        if (it == _LibFactory.end())
            return Q_NULLPTR;

        return it.value()->create(args...);
    }

    QList<IdType> getIds() const
    {
        return _LibFactory.keys();
    }

    QStringList getAliases() const
    {
        QStringList list;
        QMapIterator<typename FactoryMap::key_type, typename FactoryMap::mapped_type> iter(_LibFactory);
        while (iter.hasNext())
        {
            iter.next();
            const typename FactoryMap::mapped_type &value = iter.value();
            list.append(value->alias());
        }
        return list;
    }

    QString getAlias(const IdType &id) const
    {
        return _LibFactory[id]->alias();
    }

    qint32 count() const
    {
        return _LibFactory.count();
    }
protected:
    void registerClass(const IdType &id, AbstractFactory *p)
    {
        typename FactoryMap::iterator it = _LibFactory.find(id);
        if (it == _LibFactory.end())
            _LibFactory[id] = p;
        else
            delete p;
    }

private:
    Factory(const Factory&);
    Factory &operator = (const Factory&);
    FactoryMap _LibFactory;
};

#endif // FACTORYTEMPLATE_H
