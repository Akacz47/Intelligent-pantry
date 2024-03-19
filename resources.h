#ifndef RESOURCES_H
#define RESOURCES_H

#include <QDialog>
#include <QMessageBox>
#include "baseclass.h"
#include "mainwindow.h"

namespace Ui {
class Resources;
}

class Resources : public QDialog, public BaseClass
{
    Q_OBJECT
    MainWindow* mainWindow;

public:
    explicit Resources(QSqlDatabase* db, int id,QWidget *parent = nullptr);
    ~Resources();
private slots:
    void on_okButton_clicked();
    void updateResourceQuantity(int resourceId, int quantity, bool isIngredientFlag, QString resourceName);
    void addNewResource(QString, int quantity, bool isIngredient);
signals:
    void resourcesUpdated();
private:
    Ui::Resources *ui;
};

#endif // RESOURCES_H
