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
    bool logOutClicked;
    QString suggestedRecipeToPrepare;

    void SetTableView(const QString& str);
    enum class CurrentlyViewedData{
        Craftings,
        Ingredients,
        Resources,
        SuggestedCraftings
    };

    CurrentlyViewedData currentlyViewedData;
    void SuggestButtonSwitchEnable();

    //void ShowResources();

protected:
    void resizeEvent(QResizeEvent *event) override {
        QMainWindow::resizeEvent(event);
        resizeRowsInTableView();
    }


public:
    MainWindow(QString l, int id, QSqlDatabase* db, QDialog *parent = nullptr);
    ~MainWindow();

    //void resizeRowsInTableView();
    void setUserData(QString l, int id);


private slots:
    void on_logOutButton_clicked();

    void on_actionIngrAdd_triggered();

    void on_actionRecipesAdd_triggered();

    void on_actionResDodaj_triggered();

    void on_actionResUsun_triggered();

    void on_suggetButton_clicked();

    void on_tableView_activated(const QModelIndex &index);

    void on_deleteIngrButton_clicked();

private:
    Ui::MainWindow *ui;
public slots:
void resizeRowsInTableView();
void ShowResources();

};
#endif // MAINWINDOW_H
