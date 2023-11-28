#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QMessageBox>
#include "mainwindow.h"
#include <QCryptographicHash>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

    QSqlDatabase* db;
    MainWindow* mw;
    QString login;
    int id;

    void open_MainWindow();
    bool check_existing_users(bool checkLoginOnly=false);
    bool isLineEditEmpty();
    void encodeText(QString& l, QString& p);
    void decodeText(QString &l, QString &p);
    //QByteArray key = "0123456789abcdef";
    QString iv = "this-is-my-vector";
    QString key = "0123456789abcdef";

public:
    Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_loginButton_clicked();

    void on_registerButton_clicked();

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
