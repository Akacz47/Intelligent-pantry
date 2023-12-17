#ifndef RESOURCES_H
#define RESOURCES_H

#include <QDialog>
#include <QMessageBox>
#include "baseclass.h"

namespace Ui {
class Resources;
}

class Resources : public BaseClass
{
    Q_OBJECT

public:
    explicit Resources(QSqlDatabase* db, int id,QWidget *parent = nullptr);
    ~Resources();
private slots:
    void on_okButton_clicked();
    int getResourceQuantity(int resourceId);
    void updateResourceQuantity(int resourceId, int quantity);
    void addNewResource(int resourceId, int quantity, bool isIngredient);
    bool isIngredient(QString resourceName);
    bool isRecipe(QString resourceName);
    int getResourceId(QString resourceName, bool& isIngredient);
    int getIngredientId(QString ingredientName);
    int getRecipeId(QString recipeName);
private:
    Ui::Resources *ui;
};

#endif // RESOURCES_H
