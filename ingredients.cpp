#include "ingredients.h"
#include "ui_ingredients.h"


Ingredients::Ingredients(QSqlDatabase* db, int id, QWidget *parent) :
    BaseClass(db,id,parent),
    ui(new Ui::Ingredients)
{
    ui->setupUi(this);
    timer = new QTimer(this);

    connect(timer,&QTimer::timeout,[=]() { timer->stop(); ui->notificationLabel->setText(""); }); //[=] jest po to, aby ui było widoczne
    setWindowTitle("Składniki");
}

Ingredients::~Ingredients()
{
    delete timer;
    delete ui;
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
    QString name = ui->ingrNameLineEdit->text();
    if(name == "")
        runNotification("Wpisz nazwę składnika", false);
    else if(!doesTheIngredientExist(name)) {
        QSqlQuery query;
        query.prepare("INSERT INTO ingredients (name, userId) VALUES (?, ?)");
        query.addBindValue(name);
        query.addBindValue(id);
        query.exec();
        runNotification("Dodano składnik do bazy danych",true);
    }
    else {
        runNotification("Taki składnik już istnieje", false);
    }
}

