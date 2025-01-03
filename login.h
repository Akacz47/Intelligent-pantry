#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QMessageBox>
#include "mainwindow.h"
#include "baseclass.h"

namespace Ui {
class Login;
}

class Login : public QDialog, public BaseClass
{
    Q_OBJECT

    //QSqlDatabase* db;
    MainWindow* mw;
    QString login;
    //int id;

    void open_MainWindow();
    bool check_existing_users(bool checkLoginOnly=false);
    bool isLineEditEmpty();
    QString salt = "twsRmlJermsjpgN";

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
