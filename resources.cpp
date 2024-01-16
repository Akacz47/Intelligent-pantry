#include "resources.h"
#include "ui_resources.h"

Resources::Resources(QSqlDatabase* db, int id, QWidget *parent) :
    QDialog(parent),
    BaseClass(db, id),
    ui(new Ui::Resources)
{
    ui->setupUi(this);
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    mainWindow = qobject_cast<MainWindow*>(parent);
    if (mainWindow) {
        connect(this, SIGNAL(resourcesUpdated()), mainWindow, SLOT(ShowResources()));
        connect(this, SIGNAL(resourcesUpdated()), mainWindow, SLOT(resizeRowsInTableView()));
    }
}

Resources::~Resources()
{
    delete ui;
    mainWindow = nullptr;
}


void Resources::updateResourceQuantity(int resourceId, int quantity) {
    QSqlQuery query;
    query.prepare("UPDATE resources SET quantity = quantity + ? WHERE ingrOrCraftId = ?");
    query.addBindValue(quantity);
    query.addBindValue(resourceId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Błąd", "Nie udało się zaktualizować ilości zasobu.");
    } else {
        QMessageBox::information(this, "Sukces", "Ilość zasobu została zaktualizowana.");
        emit resourcesUpdated();
    }
}

void Resources::addNewResource(int resourceId, int quantity, bool isIngredient) {
    if (resourceId < 0) {
        QMessageBox::warning(this, "Błąd", "Nieprawidłowy ID zasobu.");
        return;
    }
    qDebug() << isIngredient;
    int ingrOrCraftBool = isIngredient ? 0 : 1;

    qDebug() << ingrOrCraftBool;
    QSqlQuery query;
    query.prepare("INSERT INTO resources (quantity, userId, ingrOrCraftId, ingrOrCraftBool) VALUES (?, ?, ?, ?)");
    query.addBindValue(quantity);
    query.addBindValue(id);
    query.addBindValue(resourceId);
    query.addBindValue(ingrOrCraftBool);

    if (!query.exec()) {
        QMessageBox::critical(this, "Błąd", "Nie udało się dodać zasobu do bazy danych.");
    } else {
        QMessageBox::information(this, "Sukces", "Zasób został dodany.");
        emit resourcesUpdated();
    }
}

void Resources::on_okButton_clicked(){
QString resourceName = ui->resourcesNameLineEdit->text();
    int quantity = ui->spinBox->value();

    if (resourceName.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Nazwa zasobu nie może być pusta.");
        return;
    }

    bool isIngredientFlag = false;
    int resourceId = getResourceId(resourceName, isIngredientFlag);

    if (resourceId == -1) {
        QMessageBox::critical(this, "Błąd", "Nie znaleziono zasobu w bazie danych.");
        return;
    }

    int currentQuantity = searchResourceQuantity(resourceName);
    if (currentQuantity > 0) {
        updateResourceQuantity(resourceId, quantity);
    } else {
        addNewResource(resourceId, quantity, isIngredientFlag);
    }
}
