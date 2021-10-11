#ifndef RENOVATION_H
#define RENOVATION_H

#include "db/dbtable.h"
#include <QVariant>
#include <QSqlDatabase>

class TRenovation : public DbTable
{
public:
    enum Type
    {
        TypePremium = 0, // Премиум
        TypeEuro, // Евро
        TypeBathroom, // Санузел
    };
    TRenovation(QSqlDatabase _db = QSqlDatabase::database());
    virtual ~TRenovation();
};

#endif // RENOVATION_H
