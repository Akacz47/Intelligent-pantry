#ifndef REMOVERECIPES_H
#define REMOVERECIPES_H

#include <QDialog>
#include <QMessageBox>
#include "baseclass.h"

namespace Ui {
class RemoveRecipes;
}

class RemoveRecipes : public QDialog, public BaseClass
{
    Q_OBJECT

public:
    explicit RemoveRecipes(QSqlDatabase* db, int id, QWidget *parent = nullptr);
    ~RemoveRecipes();
public slots:
    void on_okButton_clicked();

private:
    Ui::RemoveRecipes *ui;
};

#endif // REMOVERECIPES_H
