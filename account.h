#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "ui_baseui.h"
#include "baseclass.h"


class Account : public QDialog, private Ui::BaseUi, public BaseClass
{
    Q_OBJECT
public:
    Account(QSqlDatabase* db, int id, QWidget *parent=nullptr);
};

#endif // ACCOUNT_H
