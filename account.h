#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "ui_baseui.h"
#include "baseclass.h"
#include <QMessageBox>

class Account : public QDialog, private Ui::BaseUi, public BaseClass
{
    Q_OBJECT
    void okPressed();
    QString findOldPassword();
    QString iv = "this-is-my-vector";
    QString key = "0123456789abcdef";
public:
    Account(QSqlDatabase* db, int id, QWidget *parent=nullptr);
};

#endif // ACCOUNT_H
