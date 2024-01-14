#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QStandardItemModel>
#include <QMessageBox>
#include "baseclass.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public BaseClass
{
    Q_OBJECT

    QDialog* loginWindow;
    QString login;
    bool logOutClicked;
    QString suggestedRecipeToPrepare;
    QSqlQueryModel* model;
    QStandardItemModel* modelBezSql;
    QMap<QString, int> ingredientsOfTheRecipeMap;

    void SetTableView(QSqlQuery&);
    enum class CurrentlyViewedData{
        Craftings,
        Ingredients,
        IngredientsOfTheRecipe,
        IngredientsOfShoppingList,
        Resources,
        SuggestedCraftings
    };

    CurrentlyViewedData currentlyViewedData;
    void SuggestButtonSwitchEnable();
    void OpenWindow(QDialog*);
    void connectActionTriggered();
    void ShowIngredientsOfRecipe(QString);

public:
    MainWindow(QString l, int id, QSqlDatabase* db, QDialog *parent = nullptr);
    ~MainWindow();


private slots:
    void on_logOutButton_clicked();

    void on_suggetButton_clicked();

    void on_tableView_activated(const QModelIndex &index);

    void on_deleteIngrButton_clicked();

private:
    Ui::MainWindow *ui;

protected:
    void resizeEvent(QResizeEvent*) override;

public slots:
    void ShowResources();
    void ShowIngredients();
    void ShowRecipes();
    void resizeRowsInTableView();
    void AddIngredientsToTheShoppingList();
    void ShowShoppingList();

};
#endif // MAINWINDOW_H
