#include "baseclass.h"

BaseClass::BaseClass(QSqlDatabase* db, int id, QWidget* parent) : QDialog(parent)
{
    this->db = db;
    this->id = id;
    model = new QSqlQueryModel();
}

BaseClass::~BaseClass()
{
    delete model;
    model = nullptr;
    db = nullptr;
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
