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
    void updateResourceQuantity(int resourceId, int quantity);
    void addNewResource(int resourceId, int quantity, bool isIngredient);
public slots:
    bool isIngredient(QString resourceName);
    bool isRecipe(QString resourceName);
    int getResourceId(QString resourceName, bool& isIngredient);
    int getIngredientId(QString ingredientName);
    int getRecipeId(QString recipeName);
    int getResourceQuantity(int resourceId);
signals:
    void resourcesUpdated();
private:
    Ui::Resources *ui;
};

#endif // RESOURCES_H
