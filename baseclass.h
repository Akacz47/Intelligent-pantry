#ifndef BASECLASS_H
#define BASECLASS_H

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>

class BaseClass
{
protected:
    QSqlDatabase* db;
    int id;

    bool doesTheIngredientExist(QString);
    bool doesRecipeExist(QString);
    int searchIngrId(QString);
    int searchCraftId(QString);
    int searchResourceQuantity(QString);

public:
    BaseClass(QSqlDatabase* db, int id);
    ~BaseClass();

};

#endif // BASECLASS_H
