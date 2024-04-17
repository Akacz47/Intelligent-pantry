#include "baseui.h"
#include "ui_baseui.h"

BaseUi::BaseUi(QSqlDatabase* db, int id, QWidget *parent):
    QDialog(parent),
    BaseClass(db,id),
    ui(new Ui::BaseUi)
{
    ui->setupUi(this);
}

BaseUi::~BaseUi()
{
    delete ui;
}
