#ifndef RFID_H
#define RFID_H

#include "baseclass.h"
#include "ui_ingredients.h"
#include "ingredients.h"
#include <QDialog>
#include <QComboBox>
#include <QLabel>

class BindTag : public OperationIngredientsStrategy, public BaseClass {
public:
    BindTag(QSqlDatabase*, int, bool);
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

private:
    Ui::Ingredients *ui;
    QComboBox *comboBox;
    QLabel *label;
};

#endif // RFID_H
