#include "removerecipes.h"
#include "ui_removerecipes.h"

RemoveRecipes::RemoveRecipes(QSqlDatabase* db, int id, QWidget *parent) :
    QDialog(parent),
    BaseClass(db, id),
    ui(new Ui::RemoveRecipes)
{
    ui->setupUi(this);
    connect(ui->cancelButton, &QPushButton::clicked, this, &RemoveRecipes::reject);
}

RemoveRecipes::~RemoveRecipes()
{
    delete ui;
}


void RemoveRecipes::on_okButton_clicked() {
    QString recipeName = ui->recipeNamelineEdit->text();
    int recipeId = getRecipeId(recipeName);

    if(recipeId > 0) {
        if(deleteRecipe(recipeId)) {
            QMessageBox::information(this, "Sukces", "Przepis został pomyślnie usunięty.");
        } else {
            QMessageBox::warning(this, "Błąd", "Nie udało się usunąć przepisu. Może być on używany w zasobach lub nie masz do tego uprawnień.");
        }
    } else {
        QMessageBox::warning(this, "Błąd", "Nie znaleziono przepisu.");
    }
}
