#include "recipes.h"
#include "ui_recipes.h"
#include <QtSql/QSqlError>

Recipes::Recipes(QSqlDatabase* db, int id, QWidget *parent) :
    QDialog(parent),
    BaseClass(db,id),
    ui(new Ui::Recipes)
{
    ui->setupUi(this);
    WasOkClicled = false;
    quantityOfIngredients = 0;
}

Recipes::~Recipes()
{
    deleteItemsInVL();
    delete ui;
}

void Recipes::deleteItemsInVL()
{
    while (QLayoutItem* item = ui->gridLayout_2->takeAt(0))
        item->widget()->deleteLater();
}

void Recipes::createNIngredients()
{

    if(ui->recipeNameLineEdit->text()!="") {
        deleteItemsInVL();
        quantityOfIngredients = ui->spinBox->value();
        for(int i=0;i<quantityOfIngredients;i++){
            QLabel* label = new QLabel;
            QLineEdit* lineEdit = new QLineEdit;
            QLabel* label2 = new QLabel;
            QSpinBox* spinBox = new QSpinBox;
            label->setText("Skladnik nr. " + QString::number(i+1));
            label2->setText("Ilosc");
            spinBox->setMinimum(1);
            spinBox->setMaximum(9999);
            ui->gridLayout_2->addWidget(label, i, 0);
            ui->gridLayout_2->addWidget(lineEdit, i, 1);
            ui->gridLayout_2->addWidget(label2, i, 2);
            ui->gridLayout_2->addWidget(spinBox, i, 3);
        }
        ui->recipeNameLineEdit->setEnabled(false);
        ui->recipeNameLabel->setEnabled(false);
        ui->spinBox->setEnabled(false);
        ui->numberOfIngredientsLabel->setEnabled(false);
        WasOkClicled = true;
    }
    else {
        QMessageBox::warning(this, "Uwaga", "Wpisz nazwe przepisu", QMessageBox::Ok);
    }
}

QString Recipes::buildQuery()
{
    QString query = "INSERT INTO crafting_ingredients (ingredientId, craftingId, quantity, userId) VALUES ";
    for(int i = 0; i < quantityOfIngredients ; i++){
        if(i!=quantityOfIngredients-1)
            query += "(?, ?, ?, ?), ";
        else
            query += "(?, ?, ?, ?)";
    }
    return query;
}

void Recipes::addRecipeToDataBase()
{
    //dodaj przepis do bazy danych
    QSqlQuery query;
    query.prepare("INSERT INTO crafting (name, userId) VALUES (?, ?)");
    query.addBindValue(nameOfRecipe);
    query.addBindValue(id);
    query.exec();

    query.prepare(buildQuery());
    for(int i=0;i<quantityOfIngredients;i++){
        QWidget* widget = ui->gridLayout_2->itemAtPosition(i, 1)->widget();
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);

        QWidget* widget2 = ui->gridLayout_2->itemAtPosition(i, 3)->widget();
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(widget2);

        query.addBindValue(searchIngrId(lineEdit->text()));
        query.addBindValue(searchCraftId(nameOfRecipe));
        query.addBindValue(spinBox->value());
        query.addBindValue(id);

        lineEdit = nullptr;
        spinBox = nullptr;
        widget2 = nullptr;
        widget = nullptr;
    }
    query.exec();
    QMessageBox::information(this, "Informacja", "Dodano przepis do bazy danych", QMessageBox::Ok);

    deleteItemsInVL();
    ui->recipeNameLineEdit->setEnabled(true);
    ui->recipeNameLabel->setEnabled(true);
    ui->spinBox->setEnabled(true);
    ui->numberOfIngredientsLabel->setEnabled(true);

    ui->recipeNameLineEdit->setText("");
    ui->spinBox->setValue(1);

    ui->gridLayout_2->activate();
    ui->gridLayout_2->invalidate();

    WasOkClicled = false;
}

bool Recipes::checkQLineEdits()
{
    bool blad = false;
    QString* ingredients = new QString[quantityOfIngredients];
    for(int i=0;i<quantityOfIngredients*4;i++){
        QWidget* widget = ui->gridLayout_2->itemAt(i)->widget();
        if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget)) {
            QString name = lineEdit->text();
            ingredients->append(name);
            if(name == "") {
                blad = true;
                QMessageBox::warning(this, "Uwaga", "Uzupelnij wszystkie dane", QMessageBox::Ok);
            } else if(!doesTheIngredientExist(name)) {
                blad = true;
                QMessageBox::critical(this, "Blad", "Taki skladnik nie istnieje", QMessageBox::Ok);
            }
            for(int i=0;i<ingredients->length()-1;i++) {
                if(ingredients->at(i) == name) {
                    blad = true;
                    QMessageBox::warning(this, "Uwaga", "Skladniki nie mogą się powtarzać", QMessageBox::Ok);
                }
            }
            lineEdit = nullptr;
            if(blad) {
                delete [] ingredients;
                return false;
            }
        }
        widget = nullptr;
    }
    delete [] ingredients;
    return true;
}

void Recipes::on_okButton_clicked()
{
    if(!WasOkClicled){
        nameOfRecipe = ui->recipeNameLineEdit->text();
        if(!doesRecipeExist(nameOfRecipe)){
            createNIngredients();
        }
        else {
            ui->recipeNameLineEdit->setText("");
            QMessageBox::critical(this, "Blad", "Taki przepis juz istnieje", QMessageBox::Ok);
        }
    }
    else {
        if(checkQLineEdits())
            addRecipeToDataBase();
    }
}

