#ifndef RFID_H
#define RFID_H

#include "baseclass.h"
#include "ui_ingredients.h"
#include "ingredients.h"
#include <QDialog>
#include <QComboBox>
#include <QLabel>

class BindTag : public OperationIngredientsStrategy, public BaseClass {
    Q_OBJECT
    apiReturn rfidConnection;
    apiReturn result;
    HANDLE hScanner;
    BYTE IDBuffer[200]; //24, 100
    void writeTag(QString&);
    void setDefaultNotification();
public:
    BindTag(QSqlDatabase*, int, bool);
    ~BindTag();
    void setProduct(QString, int);
public slots:
    void execute() override;
};

class UnBindTag : public OperationIngredientsStrategy, public BaseClass {
    Q_OBJECT
    apiReturn rfidConnection;
    apiReturn result;
    HANDLE hScanner;
    BYTE IDBuffer[200]; //24, 100
    void eraseTag(QString&);
    void setDefaultNotification();
public:
    UnBindTag(QSqlDatabase*, int, bool);
    ~UnBindTag();
public slots:
    void execute() override;
};

class Rfid : public QDialog, public BaseClass
{
    Q_OBJECT
    OperationIngredientsStrategy *strategy;

public:
    ~Rfid();
    Rfid(QSqlDatabase*, int, OperationIngredientsStrategy* ois, QWidget *parent = nullptr);

private slots:
    void dataValidation();
    void switchWidgets(bool);

private:
    Ui::Ingredients *ui;
    QComboBox *comboBox;
    QLabel *label;
};

#endif // RFID_H
