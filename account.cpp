#include "account.h"

Account::Account(QSqlDatabase* db, int id, QWidget *parent):QDialog(parent),
    BaseClass(db,id) {
    setupUi(this);
    label1->setText("Stare haslo");
    label2->setText("Noew haslo");
    lineEdit1->setEchoMode(QLineEdit::Password);
    lineEdit2->setEchoMode(QLineEdit::Password);
    connect(pushButtonOk,&QPushButton::clicked,this,&Account::okPressed);
}

void Account::okPressed()
{
    QString oldPass = lineEdit1->text();
    QString newPass = lineEdit2->text();
    if(oldPass == "" || newPass == ""){
        QMessageBox::critical(this, "Błąd", "Wszystkie pola muszą być wypełnione.");
    }
    else if(oldPass == newPass) {
        QMessageBox::critical(this, "Błąd", "Stare hasło musi być inne od nowego hasła");
    }
}
