#ifndef INGREDIENTS_H
#define INGREDIENTS_H

#include "baseclass.h"
#include <QDialog>
#include <QTimer>

namespace Ui {
class Ingredients;
}

class Ingredients : public QDialog, public BaseClass
{
    Q_OBJECT


    void runNotification(QString, bool);
    QTimer* timer;

public:
    explicit Ingredients(QSqlDatabase* db, int id,QWidget *parent = nullptr);
    ~Ingredients();


private slots:
    void on_okButton_clicked();

private:
    Ui::Ingredients *ui;
};

#endif // INGREDIENTS_H
