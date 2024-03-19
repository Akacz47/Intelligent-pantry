#include "kcal.h"
#include "ui_kcal.h"

Kcal::Kcal(QSqlDatabase* db, int id, int kcal, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Kcal)
{
    this->db = db;
    this->id = id;
    ui->setupUi(this);
    ui->spinBox->setValue(kcal);
}

Kcal::~Kcal()
{
    db = nullptr;
    delete ui;
}

void Kcal::on_okButton_clicked()
{
    QSqlQuery query;
    query.prepare("UPDATE users SET kcal = ? WHERE id = ?");
    query.bindValue(0, ui->spinBox->value());
    query.bindValue(1, id);
    query.exec();
    emit newKcal(ui->spinBox->value());
    close();
}
