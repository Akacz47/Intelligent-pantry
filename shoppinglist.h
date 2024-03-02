#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#include "baseclass.h"
#include <QDialog>
#include "QMessageBox"

namespace Ui {
class ShoppingList;
}

class ShoppingList : public QDialog, public BaseClass
{
    Q_OBJECT
    QMap<QString, int> ingredientsManuallyAddedMap;

public:
    explicit ShoppingList(QSqlDatabase* db, int id, QMap<QString, int>&, QWidget *parent = nullptr);
    ~ShoppingList();

private slots:
    void on_okButton_clicked();

signals:
    void mapEmitted(const QMap<QString, int> &map);

private:
    Ui::ShoppingList *ui;
};

#endif // SHOPPINGLIST_H
