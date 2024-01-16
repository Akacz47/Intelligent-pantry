#include "baseclass.h"

BaseClass::BaseClass(QSqlDatabase* db, int id)
{
    this->db = db;
    this->id = id;
}

BaseClass::~BaseClass()
{

    db = nullptr;
    qDebug() << "Jestem w BaseClass";
}

bool BaseClass::doesTheIngredientExist(QString name)
{
    QSqlQuery query;
    query.prepare("SELECT name FROM ingredients WHERE userId IN (0,?) AND name = ?");
    query.addBindValue(id);
    query.addBindValue(name);
    query.exec();
    if(query.next()){
        return true;
    }
    return false;
}

bool BaseClass::doesRecipeExist(QString name)
{
    QSqlQuery query;
    query.prepare("SELECT name FROM crafting WHERE userId IN (0,?) AND name = ?");
    query.addBindValue(id);
    query.addBindValue(name);
    query.exec();
    if(query.next()){
        return true;
    }
    return false;
}

int BaseClass::searchIngrId(QString name)
{
    int ingrId;
    QSqlQuery query;
    query.prepare("SELECT id FROM ingredients WHERE userId IN (0,?) AND name = ?");
    query.addBindValue(id);
    query.addBindValue(name);
    query.exec();
    if(query.next()){
        ingrId = query.value(0).toInt();
        return ingrId;
    }
    return -1;
}

int BaseClass::searchCraftId(QString name)
{
    int craftId;
    QSqlQuery query;
    query.prepare("SELECT id FROM crafting WHERE userId IN (0,?) AND name = ?");
    query.addBindValue(id);
    query.addBindValue(name);
    query.exec();
    if(query.next()){
        craftId = query.value(0).toInt();
        return craftId;
    }
    return -1;
}

int BaseClass::searchResourceQuantity(QString name)
{
    int quantity;
    QSqlQuery query;
    QString question = "SELECT quantity FROM resources WHERE userId = ? AND ingrOrCraftBool = 0 AND ingrOrCraftId IN "
                       "(SELECT id FROM ingredients WHERE userId IN (0,?) AND name = ?);";
    query.prepare(question);
    query.addBindValue(id);
    query.addBindValue(id);
    query.addBindValue(name);
    query.exec();
    if(query.next()){
        quantity = query.value(0).toInt();
        return quantity;
    }
    return 0;
}

int BaseClass::getResourceQuantity(int resourceId, bool isIngredientFlag) {
    int ingrOrCraftBool = isIngredientFlag ? 0 : 1;

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM resources WHERE ingrOrCraftId = ? AND userId = ? AND ingrOrCraftBool = ?");
    checkQuery.addBindValue(resourceId);
    checkQuery.addBindValue(id);
    checkQuery.addBindValue(ingrOrCraftBool);
    checkQuery.exec();

    int count = 0;
    if (checkQuery.next()) {
        count = checkQuery.value(0).toInt();
    }

    // Jeśli zasób nie istnieje, dodaj go
    if (count == 0) { // Dodanie zasobu z domyślną ilością (np. 0)
        return 0;
    }

    // Jeśli zasób istnieje, pobierz jego ilość
    QSqlQuery query;
    query.prepare("SELECT quantity FROM resources WHERE ingrOrCraftId = ? AND userId = ? AND ingrOrCraftBool = ?");
    query.addBindValue(resourceId);
    query.addBindValue(id);
    query.addBindValue(ingrOrCraftBool);
    query.exec();

    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
int BaseClass::getResourceId(QString resourceName, bool& isIngredientFlag) {
    if (isIngredient(resourceName)) {
        isIngredientFlag = true;
        return getIngredientId(resourceName);
    } else if (isRecipe(resourceName)) {
        isIngredientFlag = false;
        return getRecipeId(resourceName);
    }
    return -1;
}

int BaseClass::getIngredientId(QString ingredientName) {
    QSqlQuery query;
    query.prepare("SELECT id FROM ingredients WHERE name = ?");
    query.addBindValue(ingredientName);
    query.exec();

    if(query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

int BaseClass::getRecipeId(QString recipeName) {
    QSqlQuery query;
    query.prepare("SELECT id FROM crafting WHERE name = ?");
    query.addBindValue(recipeName);
    query.exec();

    if(query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

bool BaseClass::isIngredient(QString resourceName) {
    QSqlQuery query;
    query.prepare("SELECT id FROM ingredients WHERE name = ?");
    query.addBindValue(resourceName);
    query.exec();

    if(query.next()) {
        return true;
    }
    return false;
}

bool BaseClass::isRecipe(QString resourceName) {
    QSqlQuery query;
    query.prepare("SELECT id FROM crafting WHERE name = ?");
    query.addBindValue(resourceName);
    query.exec();

    if(query.next()) {
        return true;
    }
    return false;
}
