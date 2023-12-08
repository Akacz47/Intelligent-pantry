#ifndef BASECLASS_H
#define BASECLASS_H

#include <QDialog>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QDialog>
#include <QTimer>

class BaseClass : public QDialog
{
protected:
    QSqlDatabase* db;
    int id;
    QSqlQueryModel* model;

    bool doesTheIngredientExist(QString name);

public:
    BaseClass(QSqlDatabase* db, int id, QWidget *parent = nullptr);
    ~BaseClass();

};

#endif // BASECLASS_H
