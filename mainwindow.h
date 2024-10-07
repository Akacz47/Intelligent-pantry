#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QStandardItemModel>
#include <QMessageBox>

#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include "baseclass.h"
#include "settings.h"


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
    QMap<QString, int> namesOfAddedRecipesToShoppingListMap;
    QMap<QString, int> ingredientsManuallyAddedMap;

    UserSettings* userSettings;

    void SetTableView(QSqlQuery&);
    enum class CurrentlyViewedData{
        Craftings,
        Ingredients,
        IngredientsOfTheRecipe,
        IngredientsOfShoppingList,
        Resources,
        SuggestedCraftings,
        RecipesOfTheShoppingList
    };

    CurrentlyViewedData currentlyViewedData;
    void SuggestButtonSwitchEnable();
    void OpenWindow(QDialog*);
    void connectActionTriggered();
    void ShowIngredientsOfRecipe(QString);
    void showRecipesOfTheShoppingList();
    void colorKcalTableView();

    void saveToFile();
    void readFromFile();

    void queryModelToStandardModel();

    //For RFID
    BYTE IDBuffer[200]; //24, 100
    apiReturn rfidConnection;
    apiReturn result;
    HANDLE hScanner;
    bool connectToRfid();
    bool disconnectFromRfid();
    void prepareToReadTags();
    void readTags();
    void stopReadingTags();
    QString findResourceByTag(const QString&);
    QVector<QString> extractSegments(const QString&);
    int nCounterRfid = 0;
    int additionalColumnsRfid = 0;
    QTimer* coolDownTimerRfid = nullptr;
    bool rfidEnabled = false;

public:
    MainWindow(QString l, int id, QSqlDatabase* db, QDialog *parent = nullptr);
    ~MainWindow();


private slots:
    void on_logOutButton_clicked();

    void on_suggetButton_clicked();

    void on_tableView_activated(const QModelIndex &index);

    void on_deleteIngrButton_clicked();

    void loadSettings();

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
    void receiveMap(const QMap<QString, int> &map) {
        ingredientsManuallyAddedMap = map;
    }

};
#endif // MAINWINDOW_H
