#include "rfid.h"

Rfid::Rfid(QSqlDatabase* db, int id, OperationIngredientsStrategy* ois, QWidget *parent) :
    QDialog(parent),
    BaseClass(db,id),
    strategy(ois),
    ui(new Ui::Ingredients)
{
    ui->setupUi(this);
    connect(ui->okButton,&QPushButton::clicked,this,&Rfid::dataValidation);
    setWindowTitle("RFID");
    strategy->setupIngredientUi(ui);

    //dodaj typ produktu (przepis czy skladnik)
    ui->ingrNameLabel->setText("Nazwa produktu");

    comboBox = new QComboBox();
    comboBox->addItem("Składnik");
    comboBox->addItem("Przepis");
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    label = new QLabel("Typ produktu");
    ui->rfidLayout->addWidget(label);
    ui->rfidLayout->addWidget(comboBox);
}

Rfid::~Rfid()
{
    delete strategy;
    delete label;
    delete comboBox;
    delete ui;
}

void Rfid::dataValidation()
{
    //if()
    //disconnect
    //strategy->execute();
}

BindTag::BindTag(QSqlDatabase *db, int id, bool useTimer) : OperationIngredientsStrategy(useTimer), BaseClass(db,id) {}

void BindTag::execute()
{

}
