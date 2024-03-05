#include "removeingredients.h"
#include "ui_removeingredients.h"


RemoveIngredients::RemoveIngredients(QSqlDatabase* db, int id, QWidget *parent) :
    QDialog(parent),
    BaseClass(db, id),
    ui(new Ui::RemoveIngredients)
{
    ui->setupUi(this);
    connect(ui->cancelButton, &QPushButton::clicked, this, &RemoveIngredients::reject);
}

RemoveIngredients::~RemoveIngredients()
{
    delete ui;
}

void RemoveIngredients::on_okButton_clicked() {
    QString ingredientName = ui->ingrNameLineEdit->text();
    int ingredientId = getIngredientId(ingredientName);
    int ownerId = getIngredientOwnerId(ingredientId);
    if(ownerId == 0) {
        QMessageBox::warning(this, "Błąd", "Nie można usunąć składnika predefiniowanego.");
        return;
    }

    if(ingredientId > 0 && ownerId == id) {
        if(isIngredientUsedInRecipes(ingredientId) || isIngredientUsedInResources(ingredientId)) {
            QMessageBox::warning(this, "Błąd", "Nie można usunąć składnika, ponieważ jest on używany.");
        } else if (deleteIngredient(ingredientId)) {
            QMessageBox::information(this, "Sukces", "Składnik został pomyślnie usunięty.");
        } else {
            QMessageBox::warning(this, "Błąd", "Nie udało się usunąć składnika.");
        }
    } else {
        QMessageBox::warning(this, "Błąd", "Składnik nie istnieje lub nie masz uprawnień do jego usunięcia.");
    }
}
