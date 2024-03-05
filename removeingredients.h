#ifndef REMOVEINGREDIENTS_H
#define REMOVEINGREDIENTS_H

#include <QDialog>
#include "baseclass.h"
#include "QMessageBox"

namespace Ui {
class RemoveIngredients;
}

class RemoveIngredients : public QDialog, public BaseClass
{
    Q_OBJECT

public:
    explicit RemoveIngredients(QSqlDatabase* db, int id, QWidget *parent = nullptr);
    ~RemoveIngredients();
public slots:
    void on_okButton_clicked();
private:
    Ui::RemoveIngredients *ui;
};

#endif // REMOVEINGREDIENTS_H
