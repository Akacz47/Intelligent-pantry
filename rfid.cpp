#include "rfid.h"
#include <QDebug>

Rfid::Rfid(QSqlDatabase* db, int id, OperationIngredientsStrategy* ois, QWidget *parent) :
    QDialog(parent),
    BaseClass(db,id),
    strategy(ois),
    ui(new Ui::Ingredients)
{
    ui->setupUi(this);
    setWindowTitle("RFID");
    strategy->setupIngredientUi(ui);
    delete ui->unitLabel;
    delete ui->unitLineEdit;
    if (ois->inherits("BindTag")){
        connect(ui->okButton,&QPushButton::clicked,this,&Rfid::dataValidation);
        connect(strategy,&OperationIngredientsStrategy::tagWritten,this,&Rfid::switchWidgets);

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
    else{
        delete ui->ingrNameLabel;
        delete ui->ingrNameLineEdit;
        delete ui->okButton;
        delete ui->rfidLayout;

        strategy->startExecuteTimer(1000);
        strategy->setDefaultNotification();
    }

}

void Rfid::switchWidgets(bool enable)
{
    ui->okButton->setEnabled(enable);
    ui->ingrNameLineEdit->setEnabled(enable);
    ui->ingrNameLabel->setEnabled(enable);
    label->setEnabled(enable);
    comboBox->setEnabled(enable);
}

Rfid::~Rfid()
{
    if (strategy->inherits("BindTag")){
        delete label;
        delete comboBox;
    }

    delete strategy;
    delete ui;
}

void Rfid::dataValidation()
{
    QString product = ui->ingrNameLineEdit->text();
    if(comboBox->currentText() == "Składnik") {
        if(doesTheIngredientExist(product)) {
            switchWidgets(false);
            strategy->setProduct(product,0);
            strategy->startExecuteTimer(1000);
            strategy->setDefaultNotification();
        }
        else{
            ui->notificationLabel->setStyleSheet("color: red;");
            ui->notificationLabel->setText("Nie znaleziono składnika");
        }
    } else{
        if(doesRecipeExist(product)) {
            switchWidgets(false);
            strategy->setProduct(product,1);
            strategy->startExecuteTimer(1000);
            strategy->setDefaultNotification();
        }
        else{
            ui->notificationLabel->setStyleSheet("color: red;");
            ui->notificationLabel->setText("Nie znaleziono przepisu");
        }
    }
    //strategy->execute();
}

void BindTag::writeTag(QString& tag)
{
    QSqlQuery query;
    query.prepare("INSERT INTO rfid (tagId, ingrOrCraftId, ingrOrCraftBool, userId) VALUES (?, ?, ?, ?)");
    query.addBindValue(tag);
    if (productType == 0)
        query.addBindValue(getIngredientId(productName));
    else
        query.addBindValue(getRecipeId(productName));
    query.addBindValue(productType);
    query.addBindValue(id);
    query.exec();
}


BindTag::BindTag(QSqlDatabase *db, int id, bool useTimer) : OperationIngredientsStrategy(useTimer), BaseClass(db,id) {
    coolDownTimer = new QTimer(this);
    connect(coolDownTimer, &QTimer::timeout, this, &BindTag::execute);


    RfidSdk::VH_InitUsb(NULL);
    rfidConnection = RfidSdk::VH_ConnectScannerUsb(&hScanner);
}

BindTag::~BindTag()
{
    delete coolDownTimer;
    coolDownTimer = nullptr;

    // Odłączenie czytnika
    rfidConnection = RfidSdk::VH_DisconnectScannerUsb(hScanner);

    // Zamknięcie połączenia USB
    rfidConnection = RfidSdk::VH_CloseUsb();
}

void BindTag::execute()
{
    if (rfidConnection == _OK) {

        int nCounter = 0;
        result = RfidSdk::EPC1G2_ReadLabelID(hScanner, 1, 0, 0, NULL, IDBuffer, &nCounter, 0);
        if (result == _OK && nCounter == 1) {
            qDebug() << "Odczytano EPC NO.:";
            QString tag = byteToString(IDBuffer,13);//5
            stopExecuteTimer();
            emit tagWritten(true);
            if(doesTagExist(tag))
                runNotification("Taki tag jest juz uzywany",false);
            else {
                writeTag(tag);
                runNotification("Tag został zapisany",true);
            }

        } else {
            qDebug() << "Błąd odczytu EPC NO.: " << result;
        }
    }

}



void UnBindTag::eraseTag(QString& tag)
{
    QSqlQuery query;
    query.prepare("DELETE FROM rfid WHERE tagId = ? AND userId = ?");
    query.addBindValue(tag);
    query.addBindValue(id);
    query.exec();
}

UnBindTag::UnBindTag(QSqlDatabase *db, int id, bool useTimer) : OperationIngredientsStrategy(useTimer), BaseClass(db,id) {
    coolDownTimer = new QTimer(this);
    connect(coolDownTimer, &QTimer::timeout, this, &UnBindTag::execute);


    RfidSdk::VH_InitUsb(NULL);
    rfidConnection = RfidSdk::VH_ConnectScannerUsb(&hScanner);
}

UnBindTag::~UnBindTag()
{
    stopExecuteTimer();

    delete coolDownTimer;
    coolDownTimer = nullptr;

    // Odłączenie czytnika
    rfidConnection = RfidSdk::VH_DisconnectScannerUsb(hScanner);

    // Zamknięcie połączenia USB
    rfidConnection = RfidSdk::VH_CloseUsb();
}

void UnBindTag::execute()
{
    if (rfidConnection == _OK) {

        int nCounter = 0;
        result = RfidSdk::EPC1G2_ReadLabelID(hScanner, 1, 0, 0, NULL, IDBuffer, &nCounter, 0);
        if (result == _OK && nCounter == 1) {
            QString tag = byteToString(IDBuffer,13);//5
            if(doesTagExist(tag)){
                eraseTag(tag);
                runNotification("Tag został usuniety",true);
            }
            else
                runNotification("Taki tag nie jest uzywany",false);

        } else {
            qDebug() << "Błąd odczytu EPC NO.: " << result;
        }
    }

}
