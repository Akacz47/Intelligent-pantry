#include "ingredients.h"
#include "ui_ingredients.h"


Ingredients::Ingredients(OperationIngredientsStrategy* ois, QWidget *parent) :
    QDialog(parent),
    strategy(ois),
    ui(new Ui::Ingredients)
{
    ui->setupUi(this);
    connect(ui->okButton,&QPushButton::clicked,this,&Ingredients::okButton);
    setWindowTitle("Składniki");
    strategy->setupIngredientUi(ui);
}

Ingredients::~Ingredients()
{
    delete strategy;
    delete ui;
}

void Ingredients::okButton()
{
    strategy->execute();
}

OperationIngredientsStrategy::~OperationIngredientsStrategy() {
    ui = nullptr;
    timer = nullptr;
    if (useTimer)
        delete timer;
}

void OperationIngredientsStrategy::runNotification(QString text, bool good)
{
    if(good)
        ui->notificationLabel->setStyleSheet("color: green;");
    else
        ui->notificationLabel->setStyleSheet("color: red;");
    ui->notificationLabel->setText(text);
    if(useTimer)
        timer->start(3000);
}

void OperationIngredientsStrategy::setupIngredientUi(Ui::Ingredients *ui)
{
    this->ui = ui;
    if (useTimer){
        timer = new QTimer(this);
        connect(timer,&QTimer::timeout,[&]() { timer->stop(); this->ui->notificationLabel->setText(""); }); //[=] jest po to, aby ui było widoczne
    }
}

AddIngredients::AddIngredients(QSqlDatabase *db, int id) : BaseClass(db,id) {}
RemoveIngredients::RemoveIngredients(QSqlDatabase *db, int id) : BaseClass(db,id) {}

void AddIngredients::execute() {
    QString name = ui->ingrNameLineEdit->text();
    if(name == "")
        runNotification("Wpisz nazwę składnika", false);
    else if(!doesTheIngredientExist(name)) {
        QSqlQuery query;
        query.prepare("INSERT INTO ingredients (name, userId) VALUES (?, ?)");
        query.addBindValue(name);
        query.addBindValue(id);
        query.exec();
        runNotification("Składnik został dodany do bazy danych", true);
    }
    else
        runNotification("Taki składnik już istnieje", false);
}

void RemoveIngredients::execute() {
    QString ingredientName = ui->ingrNameLineEdit->text();
    int ingredientId = getIngredientId(ingredientName);
    int ownerId = getIngredientOwnerId(ingredientId);
    if(ownerId == 0) {
        runNotification("Nie można usunąć składnika predefiniowanego.",false);
        return;
    }

    if(ingredientId > 0 && ownerId == id) {
        if(isIngredientUsedInRecipes(ingredientId) || isIngredientUsedInResources(ingredientId)) {
            runNotification("Nie można usunąć składnika, ponieważ jest on używany.",false);
        } else if (deleteIngredient(ingredientId)) {
            runNotification("Składnik został pomyślnie usunięty.",true);
        } else {
            runNotification("Nie udało się usunąć składnika.",false);
        }
    } else {
        runNotification("Składnik nie istnieje lub nie masz uprawnień do jego usunięcia.",false);
    }
}
