#include "resources.h"
#include "ui_resources.h"

Resources::Resources(QSqlDatabase* db, int id, QWidget *parent) :
    BaseClass(db, id, parent),
    ui(new Ui::Resources)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(on_okButton_clicked()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    ui->spinBox->setMinimum(1);
    ui->spinBox->setMaximum(9999);
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(onSpinBoxValueChanged(int)));
}

Resources::~Resources()
{
    delete ui;
}


int Resources::getResourceQuantity(int resourceId) {
    QSqlQuery query;
    query.prepare("SELECT quantity FROM resources WHERE ingrOrCraftId = ? AND userId = ?");
    query.addBindValue(resourceId);
    query.addBindValue(id);
    query.exec();

    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

void Resources::updateResourceQuantity(int resourceId, int quantity) {
    QSqlQuery query;
    query.prepare("UPDATE resources SET quantity = quantity + ? WHERE id = ?");
    query.addBindValue(quantity);
    query.addBindValue(resourceId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Błąd", "Nie udało się zaktualizować ilości zasobu.");
    } else {
        QMessageBox::information(this, "Sukces", "Ilość zasobu została zaktualizowana.");
    }
}
bool Resources::isIngredient(QString resourceName) {
    QSqlQuery query;
    query.prepare("SELECT id FROM ingredients WHERE name = ?");
    query.addBindValue(resourceName);
    query.exec();

    if(query.next()) {
        return true;
    }
    return false;
}

bool Resources::isRecipe(QString resourceName) {
    QSqlQuery query;
    query.prepare("SELECT id FROM crafting WHERE name = ?");
    query.addBindValue(resourceName);
    query.exec();

    if(query.next()) {
        return true;
    }
    return false;
}

int Resources::getResourceId(QString resourceName, bool& isIngredientFlag) {
    if (isIngredient(resourceName)) {
        isIngredientFlag = true;
        return getIngredientId(resourceName);
    } else if (isRecipe(resourceName)) {
        isIngredientFlag = false;
        return getRecipeId(resourceName);
    }
    return -1;
}

int Resources::getIngredientId(QString ingredientName) {
    QSqlQuery query;
    query.prepare("SELECT id FROM ingredients WHERE name = ?");
    query.addBindValue(ingredientName);
    query.exec();

    if(query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

int Resources::getRecipeId(QString recipeName) {
    QSqlQuery query;
    query.prepare("SELECT id FROM crafting WHERE name = ?");
    query.addBindValue(recipeName);
    query.exec();

    if(query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}
void Resources::addNewResource(int resourceId, int quantity, bool isIngredient) {
    if (resourceId < 0) {
        QMessageBox::warning(this, "Błąd", "Nieprawidłowy ID zasobu.");
        return;
    }

    int ingrOrCraftBool = isIngredient ? 0 : 1;

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

    int currentQuantity = getResourceQuantity(resourceId);
    if (currentQuantity > 0) {
        updateResourceQuantity(resourceId, currentQuantity + quantity);
    } else {
        addNewResource(resourceId, quantity, isIngredientFlag);
    }

    //this->accept();
}
