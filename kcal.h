#ifndef KCAL_H
#define KCAL_H

#include <QDialog>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

namespace Ui {
class Kcal;
}

class Kcal : public QDialog
{
    Q_OBJECT
    QSqlDatabase* db;
    int id;

public:
    explicit Kcal(QSqlDatabase*,int,int, QWidget *parent = nullptr);
    ~Kcal();

private slots:
    void on_okButton_clicked();

signals:
    void newKcal(const int&);

private:
    Ui::Kcal *ui;
};

#endif // KCAL_H
