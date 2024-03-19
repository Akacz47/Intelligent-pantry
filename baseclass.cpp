#include "baseclass.h"

BaseClass::BaseClass(QSqlDatabase* db, int id)
{
    this->db = db;
    this->id = id;

    //unit test
    if(doesRecipeExist("Spaghetti Bolognese")){
        qDebug() << "unit test: ok";
    }
    else{
        qDebug() << "unit test: blad";
    }
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
