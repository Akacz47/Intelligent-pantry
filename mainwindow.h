#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QMessageBox>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QSqlDatabase* db;
    QDialog* loginWindow;
    int id;
    QString login;
    QSqlQueryModel *model;

    void SetTableView(const QString& str);
    void ShowResources();

protected:
    void resizeEvent(QResizeEvent *event) override {
        QMainWindow::resizeEvent(event);
        resizeRowsInTableView();
    }

public:
    MainWindow(QString l, int id, QSqlDatabase* db, QDialog *parent = nullptr);
    ~MainWindow();

    void resizeRowsInTableView();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
