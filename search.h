#ifndef SEARCH_H
#define SEARCH_H

#include <QDialog>
#include <QMessageBox>
#include "baseclass.h"

namespace Ui {
class Search;
}

class Search : public QDialog, public BaseClass
{
    Q_OBJECT

public:
    explicit Search(QSqlDatabase* db, int id, QWidget *parent = nullptr);
    ~Search();
private slots:
    void on_searchButton_clicked();
private:
    Ui::Search *ui;
};

#endif // SEARCH_H
