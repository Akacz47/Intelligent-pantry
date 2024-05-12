#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QSqlDatabase* db, int id, UserSettings* us, QWidget *parent)
    : QDialog(parent)
    , BaseClass(db, id)
    , ui(new Ui::Settings)
{
    this->us = us;
    ui->setupUi(this);
    ui->kcalSpinBox->setValue(us->maxKcal);
    ui->rfidCheckBox->setChecked(us->rfidBool);
    setWindowTitle("Ustawienia");
}

Settings::~Settings()
{
    us = nullptr;
    delete ui;
}

void Settings::on_okButton_clicked()
{
    QSqlQuery query;
    query.prepare("UPDATE users SET kcal = ?, rfidBool = ? WHERE id = ?");
    query.bindValue(0, ui->kcalSpinBox->value());
    if(ui->rfidCheckBox->isChecked())
        query.bindValue(1, 1);
    else
        query.bindValue(1, 0);
    query.bindValue(2, id);
    query.exec();
    us->maxKcal = ui->kcalSpinBox->value();
    us->rfidBool = ui->rfidCheckBox->isChecked();
    close();
}

