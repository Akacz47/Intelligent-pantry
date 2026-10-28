#include "removeresources.h"
#include "ui_removeresources.h"
#include "baseclass.h"
#include "mainwindow.h"

RemoveResources::RemoveResources(QSqlDatabase* db, int id, QWidget *parent) :
    QDialog(parent),
    BaseClass(db, id),
    ui(new Ui::RemoveResources)
{
    ui->setupUi(this);
    ui->spinBox->value();
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    MainWindow* mainWindow = qobject_cast<MainWindow*>(parent);
    if (mainWindow) {
        connect(this, SIGNAL(resourcesUpdated()), mainWindow, SLOT(ShowResources()));
        connect(this, SIGNAL(resourcesUpdated()), mainWindow, SLOT(resizeRowsInTableView()));
    }
    setWindowTitle("Zasoby");
}

RemoveResources::~RemoveResources()
{
    delete ui;
}

void RemoveResources::removeResource(int resourceId, int quantityToRemove, bool isIngredientFlag) {
    if (quantityToRemove <= 0) {
        QMessageBox::warning(this, "Błąd", "Ilość do usunięcia musi być większa od 0.");
        return;
    }

    int currentQuantity = getResourceQuantity(resourceId, isIngredientFlag);
    if (currentQuantity < quantityToRemove) {
        QMessageBox::warning(this, "Błąd", "Nie można usunąć więcej zasobów niż jest dostępne.");
        return;
    }

    int ingrOrCraftBool = isIngredientFlag ? 0 : 1;
    if (currentQuantity == quantityToRemove) {
        removeAllResource(resourceId, isIngredientFlag);
    } else {
        QSqlQuery query;
        query.prepare("UPDATE resources SET quantity = quantity - ? WHERE ingrOrCraftId = ? AND userId = ? AND ingrOrCraftBool = ?");
        query.addBindValue(quantityToRemove);
        query.addBindValue(resourceId);
        query.addBindValue(id);
        query.addBindValue(ingrOrCraftBool);

        if (!query.exec()) {
            QMessageBox::critical(this, "Błąd", "Nie udało się zaktualizować ilości zasobu.");
        } else {
            QMessageBox::information(this, "Sukces", "Ilość zasobu została zaktualizowana.");
            emit resourcesUpdated();
        }
    }
}

void RemoveResources::removeAllResource(int resourceId, bool isIngredientFlag) {
    int ingrOrCraftBool = isIngredientFlag ? 0 : 1;
    QSqlQuery query;
    query.prepare("DELETE FROM resources WHERE ingrOrCraftId = ? AND userId = ? AND ingrOrCraftBool = ?");
    query.addBindValue(resourceId);
    query.addBindValue(id);
    query.addBindValue(ingrOrCraftBool);

    if (!query.exec()) {
        QMessageBox::critical(this, "Błąd", "Nie udało się usunąć zasobu.");
    } else {
        QMessageBox::information(this, "Sukces", "Zasób został usunięty.");
        emit resourcesUpdated();
    }
}

void RemoveResources::on_removeAllButton_clicked()
{
    QString resourceName = ui->resourcesNameLineEdit->text();
    bool isIngredientFlag = false;
    if (resourceName.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Nazwa zasobu nie może być pusta.");
        return;
    }

    int resourceId = getResourceId(resourceName, isIngredientFlag);
    if (resourceId < 0) {
        QMessageBox::warning(this, "Błąd", "Nie znaleziono zasobu w bazie danych.");
        return;
    }

    removeAllResource(resourceId, isIngredientFlag);
}



void RemoveResources::on_okButton_clicked()
{
    QString resourceName = ui->resourcesNameLineEdit->text();
    bool isIngredientFlag = false;
    if (resourceName.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Nazwa zasobu nie może być pusta.");
        return;
    }

    int resourceId = getResourceId(resourceName, isIngredientFlag);
    if (resourceId < 0) {
        QMessageBox::warning(this, "Błąd", "Nie znaleziono zasobu w bazie danych.");
        return;
    }

    int quantityToRemove = ui->spinBox->value();
    removeResource(resourceId, quantityToRemove, isIngredientFlag);
}

