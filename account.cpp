#include "account.h"

Account::Account(QSqlDatabase* db, int id, QWidget *parent):QDialog(parent),
    BaseClass(db,id) {
    setupUi(this);
    label1->setText("Aktualne haslo");
    label2->setText("Nowe haslo");
    lineEdit1->setEchoMode(QLineEdit::Password);
    lineEdit2->setEchoMode(QLineEdit::Password);
    connect(pushButtonOk,&QPushButton::clicked,this,&Account::okPressed);
    setWindowTitle("Konto");
}

void Account::okPressed()
{
    QString curPassUser = lineEdit1->text();
    QString newPass = lineEdit2->text();

    QString curPassUserHashed = (QCryptographicHash::hash((curPassUser + salt).toUtf8(), QCryptographicHash::Sha256)).toHex();
    QString newPassHashed = (QCryptographicHash::hash((newPass + salt).toUtf8(), QCryptographicHash::Sha256)).toHex();

    QString* userData = findOldData();

    if(curPassUser == "" || newPass == "")
        QMessageBox::critical(this, "Błąd", "Wszystkie pola muszą być wypełnione.");
    else if(userData[1] == newPassHashed)
        QMessageBox::critical(this, "Błąd", "Nowe hasło nie może być takie samo jak aktualne hasło.");
    else if(curPassUser == newPass)
        QMessageBox::critical(this, "Błąd", "Aktualne i nowe hasło nie może być takie same.");
    else if(curPassUserHashed != userData[1])
        QMessageBox::critical(this, "Błąd", "Aktualne hasło jest niepoprawne.");
    else if(userData[0] == newPassHashed)
        QMessageBox::critical(this, "Błąd", "Nowe hasło nie może być takie samo jak poprzednio używane hasło.");
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
        query.addBindValue(newPassHashed);
        query.addBindValue(id);
        query.exec();
        QMessageBox::information(this, "Informacja", "Hasło zostało zmienione" ,QMessageBox::Ok);
        this->close();
    }
    delete[] userData;
    userData = nullptr;
}

QString* Account::findOldData()
{
    QSqlQuery query;
    query.prepare("SELECT oldPassword, currentPassword FROM users WHERE id = ?");
    query.addBindValue(id);
    query.exec();
    query.next();
    QString* data = new QString[2];
    data[0] = query.value(0).toString(); // old password
    data[1] = query.value(1).toString(); // current password
    return data;
}
