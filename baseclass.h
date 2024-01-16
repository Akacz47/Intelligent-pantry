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
    bool ingrOrRec;

    bool doesTheIngredientExist(QString);
    bool doesRecipeExist(QString);
    int searchIngrId(QString);
    int searchCraftId(QString);
    int searchResourceQuantity(QString);

public:
    BaseClass(QSqlDatabase* db, int id);
    ~BaseClass();
public slots:
    bool isIngredient(QString resourceName);
    bool isRecipe(QString resourceName);
    int getResourceId(QString resourceName, bool& isIngredient);
    int getIngredientId(QString ingredientName);
    int getRecipeId(QString recipeName);
    int getResourceQuantity(int resourceId,bool);
    bool checkResourceType(int,bool);
};

#endif // BASECLASS_H
