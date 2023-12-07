#ifndef INGREDIENTS_H
#define INGREDIENTS_H

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QDialog>
#include <QTimer>

namespace Ui {
class Ingredients;
}

class Ingredients : public QDialog
{
    Q_OBJECT
    QSqlDatabase* db;
    QSqlQueryModel* model;
    int id;
    QTimer* timer;

    void runNotification(QString, bool);
    bool doesTheIngredientExist();

public:
    explicit Ingredients(QSqlDatabase* db, int id,QWidget *parent = nullptr);
    ~Ingredients();


private slots:
    void on_okButton_clicked();

private:
    Ui::Ingredients *ui;
};

#endif // INGREDIENTS_H
