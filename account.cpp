#include "account.h"

Account::Account(QSqlDatabase* db, int id, QWidget *parent):QDialog(parent),
    BaseClass(db,id) {
    setupUi(this);
    label1->setText("aaa");
    label2->setText("bbb");
}
