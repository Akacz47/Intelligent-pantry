#include "ingredients.h"
#include "ui_ingredients.h"


Ingredients::Ingredients(QSqlDatabase* db, int id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Ingredients)
{
    ui->setupUi(this);
    this->db = db;
    this->id = id;
    model = new QSqlQueryModel();
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,[=]() { timer->stop(); ui->notificationLabel->setText(""); }); //[=] jest po to, aby ui było widoczne
    setWindowTitle("Składniki");
}

Ingredients::~Ingredients()
{
    delete model;
    delete timer;
    db = nullptr;
    delete ui;
}

bool Ingredients::doesTheIngredientExist()
{
    QSqlQuery query;
    query.prepare("SELECT name FROM ingredients WHERE userId IN (0,?) AND name = ?");
    query.addBindValue(id);
    query.addBindValue(ui->ingrNameLineEdit->text());
    query.exec();
    if(query.next()){
        return true;
    }
    return false;
}

void Ingredients::runNotification(QString text, bool good)
{
    if(good)
        ui->notificationLabel->setStyleSheet("color: green;");
    else
        ui->notificationLabel->setStyleSheet("color: red;");
    ui->notificationLabel->setText(text);
    timer->start(3000);
}

void Ingredients::on_okButton_clicked()
{
    if(ui->ingrNameLineEdit->text() == "")
        runNotification("Wpisz nazwę składnika", false);
    else if(!doesTheIngredientExist()) {
        QSqlQuery query;
        query.prepare("INSERT INTO ingredients (name, userId) VALUES (?, ?)");
        query.addBindValue(ui->ingrNameLineEdit->text());
        query.addBindValue(id);
        query.exec();
        runNotification("Dodano składnik do bazy danych",true);
    }
    else {
        runNotification("Taki składnik już istnieje", false);
    }
}

