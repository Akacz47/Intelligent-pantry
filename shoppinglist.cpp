#include "shoppinglist.h"
#include "ui_shoppinglist.h"

ShoppingList::ShoppingList(QSqlDatabase* db, int id, QMap<QString, int>& mapa,QWidget *parent) :
    QDialog(parent),
    BaseClass(db,id),
    ui(new Ui::ShoppingList)
{
    ui->setupUi(this);
    ingredientsManuallyAddedMap = mapa;
    setWindowTitle("Lista zakupów");
}

ShoppingList::~ShoppingList()
{
    delete ui;
}

void ShoppingList::on_okButton_clicked()
{
    QString ingredient = ui->ingredientLineEdit->text();
    int quantity = ui->spinBox->value();
    if(doesTheIngredientExist(ingredient) || ingredient != ""){
        if (!ingredientsManuallyAddedMap.contains(ingredient))
            ingredientsManuallyAddedMap.insert(ingredient,quantity);
        else
            ingredientsManuallyAddedMap[ingredient] += quantity;
        emit mapEmitted(ingredientsManuallyAddedMap);
        QMessageBox::information(this, "Informacja", "Dodano składnik do listy zakupów", QMessageBox::Ok);
    }
    else
        QMessageBox::critical(this, "Blad", "Taki skladnik nie istnieje", QMessageBox::Ok);
}

