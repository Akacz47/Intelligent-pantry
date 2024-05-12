#ifndef INGREDIENTS_H
#define INGREDIENTS_H

#include "baseclass.h"
#include <QDialog>
#include <QTimer>

namespace Ui {
class Ingredients;
}

// Interfejs strategii
class OperationIngredientsStrategy : public QObject {
    Q_OBJECT
public:
    virtual void execute() = 0;
    virtual ~OperationIngredientsStrategy();
    void setupIngredientUi(Ui::Ingredients* ui);
protected:
    void runNotification(QString, bool);
    QTimer* timer;
    Ui::Ingredients *ui;
};

// Konkretna strategia dla dodawania składników
class AddIngredients : public OperationIngredientsStrategy, public BaseClass {
public:
    AddIngredients(QSqlDatabase* db, int id);
    void execute() override;
};

// Konkretna strategia dla usuwania składników
class RemoveIngredients : public OperationIngredientsStrategy, public BaseClass {
public:
    RemoveIngredients(QSqlDatabase* db, int id);
    void execute() override;
};

class Ingredients : public QDialog
{
    Q_OBJECT
    OperationIngredientsStrategy *strategy;

public:
    explicit Ingredients(OperationIngredientsStrategy* ois, QWidget *parent = nullptr);
    ~Ingredients();

private slots:
    void okButton();

private:
    Ui::Ingredients *ui;
};

#endif // INGREDIENTS_H
