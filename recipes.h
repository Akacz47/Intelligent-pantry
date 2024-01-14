#ifndef RECIPES_H
#define RECIPES_H

#include "baseclass.h"
#include "QMessageBox"
#include "QLineEdit"

namespace Ui {
class Recipes;
}

class Recipes : public QDialog, public BaseClass
{
    Q_OBJECT

    void deleteItemsInVL();
    void createNIngredients();
    bool WasOkClicled;
    void addRecipeToDataBase();
    int quantityOfIngredients;
    bool checkQLineEdits();
    QString nameOfRecipe;
    QString buildQuery();

public:
    explicit Recipes(QSqlDatabase* db, int id, QWidget *parent = nullptr);
    ~Recipes();

private slots:
    void on_okButton_clicked();

private:
    Ui::Recipes *ui;
};

#endif // RECIPES_H
