#include "ingredients.h"
#include "ui_ingredients.h"
#include <QDebug>

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
    delete timer;
    if (coolDownTimer != nullptr)
        delete coolDownTimer;
    timer = nullptr;
    coolDownTimer = nullptr;
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

void OperationIngredientsStrategy::setDefaultNotification() {
    ui->notificationLabel->setStyleSheet("color: green;");
    ui->notificationLabel->setText("Przybliz tag do czytnika");
}

// For RFID
void OperationIngredientsStrategy::setProduct(QString product, int type)
{
    productName = product;
    productType = type;
}

void OperationIngredientsStrategy::setupIngredientUi(Ui::Ingredients *ui)
{
    this->ui = ui;
    timer = new QTimer(this);
    if (useTimer){
        connect(timer,&QTimer::timeout,[&]() { timer->stop(); this->ui->notificationLabel->setText(""); emit clearedLabel(); }); //[&] jest po to, aby ui było widoczne
    }
}

void OperationIngredientsStrategy::startExecuteTimer(int mSec)
{
    connect(this, &OperationIngredientsStrategy::clearedLabel, this, &OperationIngredientsStrategy::setDefaultNotification);
    coolDownExecute = mSec;
    coolDownTimer->start(mSec);
}

void OperationIngredientsStrategy::stopExecuteTimer()
{
    disconnect(this, &OperationIngredientsStrategy::clearedLabel, this, &OperationIngredientsStrategy::setDefaultNotification);
    coolDownTimer->stop();
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
