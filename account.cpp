#include "account.h"

Account::Account(QSqlDatabase* db, int id, QWidget *parent):QDialog(parent),
    BaseClass(db,id) {
    setupUi(this);
    label1->setText("Stare haslo");
    label2->setText("Nowe haslo");
    lineEdit1->setEchoMode(QLineEdit::Password);
    lineEdit2->setEchoMode(QLineEdit::Password);
    connect(pushButtonOk,&QPushButton::clicked,this,&Account::okPressed);
    setWindowTitle("Konto");
}

void Account::okPressed()
{
    QString oldPassUser = lineEdit1->text();
    QString oldPassReal = findOldPassword();
    QString newPass = lineEdit2->text();
    if(oldPassUser == "" || newPass == "")
        QMessageBox::critical(this, "Błąd", "Wszystkie pola muszą być wypełnione");
    else if(oldPassReal == newPass)
        QMessageBox::critical(this, "Błąd", "Stare hasło musi być inne od nowego hasła");
    else if(oldPassUser == newPass)
        QMessageBox::critical(this, "Błąd", "Stare i nowe hasło nie może być takie same");
    else if(oldPassUser != oldPassReal)
        QMessageBox::critical(this, "Błąd", "Stare hasło jest niepoprawne");
    else if(!checkPassword(newPass)) {
        QMessageBox::warning(this, "Warning", "Hasło nie spełnia wymagań, ponieważ musi zawierać minimum:"
                                                 "\n-10 znaków."
                                                 "\n-jedną dużą i małą literę."
                                                 "\n-jedną cyfrę."
                                                 "\n-jeden znak specjalny.");
    }
    else {
        QSqlQuery query;
        query.prepare("UPDATE users SET oldPassword = currentPassword WHERE id = ?");
        query.addBindValue(id);
        query.exec();

        query.prepare("UPDATE users SET currentPassword = ? WHERE id = ?");
        encodeText(key,iv,newPass);
        query.addBindValue(newPass);
        query.addBindValue(id);
        query.exec();
        QMessageBox::information(this, "Informacja", "Hasło zostało zmienione" ,QMessageBox::Ok);
        this->close();
    }
}

QString Account::findOldPassword()
{
    QSqlQuery query;
    query.prepare("SELECT oldPassword FROM users WHERE id = ?");
    query.addBindValue(id);
    query.exec();
    query.next();
    QString oldPass = query.value(0).toString();
    decodeText(key,iv,oldPass);
    return oldPass;
}
