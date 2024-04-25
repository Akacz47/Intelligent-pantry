#include "login.h"
#include "ui_login.h"
#include <QDebug>

Login::Login(QWidget *parent) :
    QDialog(parent),BaseClass(nullptr,-1),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    mw = nullptr;
    ui->lineEditPassword->setEchoMode(QLineEdit::Password);
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
    db->setHostName("db.zut.edu.pl");
    db->setUserName("pa51074");
    db->setPassword("rauimqxG");
    db->setDatabaseName("pa51074");
    if(db->open())
        qDebug() << "Podłączono do bazy danych";
    setWindowTitle("Logowanie");
}

Login::~Login()
{
    if(db != nullptr){
        db->close();
        delete db;
        db = nullptr;
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
        qDebug() << "Baza danych odlaczona";
    }
    mw=nullptr;
    delete ui;
    qDebug() << "Jestem w Logowaniu";
}

void Login::open_MainWindow()
{
    mw = nullptr;
    hide();
    mw = new MainWindow(login,id,db,this);
    mw->show();
    mw->resizeRowsInTableView();
}

bool Login::check_existing_users(bool checkLoginOnly)
{
    login = ui->lineEditLogin->text();
    QString password = ui->lineEditPassword->text();
    QSqlQuery query;
    query.prepare("SELECT id, login, currentPassword FROM users");
    query.exec();

    QString encryptedLogin;
    QString encryptedPassword;
    while (query.next()) {
        encryptedLogin = query.value(1).toByteArray();
        encryptedPassword = query.value(2).toByteArray();
        decodeText(key,iv,encryptedLogin);
        decodeText(key,iv,encryptedPassword);
        if(checkLoginOnly && login == encryptedLogin)
            return true;
        else {
            if(login == encryptedLogin && password == encryptedPassword){
                id = query.value(0).toInt();
                return true;
            }
        }
    }
    return false;
}

bool Login::isLineEditEmpty()
{
    if(ui->lineEditLogin->text() == "" || ui->lineEditPassword->text() == "") {
        ui->lineEditLogin->setText("");
        ui->lineEditPassword->setText("");
        return true;
    }
    return false;
}

void Login::on_loginButton_clicked(){
    if(isLineEditEmpty())
        QMessageBox::warning(nullptr, "Warning", "Proszę uzupełnić wszystkie pola" ,QMessageBox::Ok);
    else if(check_existing_users())
        open_MainWindow();
    else {
        ui->lineEditLogin->setText("");
        ui->lineEditPassword->setText("");
        QMessageBox::critical(nullptr, "Error", "Zły login lub hasło.", QMessageBox::Ok);
    }
}

void Login::on_registerButton_clicked()
{
    QString password = ui->lineEditPassword->text();
    if(isLineEditEmpty())
        QMessageBox::warning(nullptr, "Warning", "Proszę uzupełnić wszystkie pola" ,QMessageBox::Ok);
    else if(check_existing_users(true))
        QMessageBox::critical(nullptr, "Error", "Taki użytkownik już istnieje", QMessageBox::Ok);
    else if(!checkPassword(password))
        QMessageBox::warning(nullptr, "Warning", "Hasło nie spełnia wymagań, ponieważ musi zawierać minimum:"
                                                 "\n-10 znaków."
                                                 "\n-jedną dużą i małą literę."
                                                 "\n-jedną cyfrę."
                                                 "\n-jeden znak specjalny." ,QMessageBox::Ok);
    else {
        QSqlQuery query;
        query.prepare("INSERT INTO users (login, oldPassword, currentPassword, kcal) VALUES (?, ?, ?, ?)");

        QString login = ui->lineEditLogin->text();
        encodeText(key,iv,login);
        encodeText(key,iv,password);

        query.bindValue(0, login);
        query.bindValue(1, password);
        query.bindValue(2, password);
        query.bindValue(3, 2000); //srednia ilosc kalorii spozywana przez czlowieka dziennie
        query.exec();
        QMessageBox::information(nullptr, "Baza danych", "Rejestracja przebiegła pomyślnie", QMessageBox::Ok);
    }
}

