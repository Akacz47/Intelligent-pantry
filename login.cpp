#include "login.h"
#include "ui_login.h"
#include "qaesencryption.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    mw = nullptr;
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
    db->setHostName("db.zut.edu.pl");
    db->setUserName("pa51074");
    db->setPassword("rauimqxG");
    db->setDatabaseName("pa51074");
    if(db->open()){
        //QMessageBox::information(nullptr, "Baza danych", "Udało się podłączyć do bazy danych.", QMessageBox::Ok);
        qDebug() << "Podłączono do bazy danych";
    }
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
    query.prepare("SELECT * FROM users");
    query.exec();

    QString encryptedLogin;
    QString encryptedPassword;
    while (query.next()) {
        encryptedLogin = query.value(1).toByteArray();
        encryptedPassword = query.value(2).toByteArray();
        decodeText(encryptedLogin,encryptedPassword);
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

void Login::encodeText(QString& l, QString& p)
{
    QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
    QByteArray hashIV = QCryptographicHash::hash(iv.toLocal8Bit(), QCryptographicHash::Md5);
    //QAESEncryption encryption(QAESEncryption::AES_128, QAESEncryption::ECB);
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    QByteArray encodedLogin = encryption.encode(l.toUtf8(), hashKey, hashIV).toHex();
    QByteArray encodedPassword = encryption.encode(p.toUtf8(), hashKey, hashIV).toHex();
    l = encodedLogin;
    p = encodedPassword;
}

void Login::decodeText(QString &l, QString &p)
{
    QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
    QByteArray hashIV = QCryptographicHash::hash(iv.toLocal8Bit(), QCryptographicHash::Md5);
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    QByteArray decodedLogin = encryption.decode(QByteArray::fromHex(l.toUtf8()), hashKey, hashIV);
    QByteArray decodedPassword = encryption.decode(QByteArray::fromHex(p.toUtf8()), hashKey, hashIV);
    l = QString(encryption.removePadding(decodedLogin));
    p = QString(encryption.removePadding(decodedPassword));
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
    if(isLineEditEmpty())
        QMessageBox::warning(nullptr, "Warning", "Proszę uzupełnić wszystkie pola" ,QMessageBox::Ok);
    else if(check_existing_users(true))
        QMessageBox::critical(nullptr, "Error", "Taki użytkownik już istnieje", QMessageBox::Ok);
    else {
        QSqlQuery query;
        query.prepare("INSERT INTO users (login, password) VALUES (?, ?)");

        QString login = ui->lineEditLogin->text();
        QString password = ui->lineEditPassword->text();
        encodeText(login, password);

        query.bindValue(0, login);
        query.bindValue(1, password);
        query.exec();
        QMessageBox::information(nullptr, "Baza danych", "Rejestracja przebiegła pomyślnie", QMessageBox::Ok);
    }
}

