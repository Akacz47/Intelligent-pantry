#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ingredients.h"
#include "recipes.h"
#include "resources.h"

MainWindow::MainWindow(QString l, int id, QSqlDatabase* db, QDialog *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUserData(l,id);
    this->db = db;
    loginWindow = parent;
    setAttribute(Qt::WA_DeleteOnClose);
    ui->welcomeLabel->setText("Witaj " + this->login);
    setWindowTitle("Spizarka");
    logOutClicked = false;

    model = new QSqlQueryModel();
    ShowResources();
}

void MainWindow::SetTableView(const QString& str)
{
    model->setQuery(std::move(str));
    ui->tableView->setModel(model);
    resizeRowsInTableView();
}

void MainWindow::ShowResources()
{
//    QString question = "SELECT podzapytanie.name AS 'nazwa', podzapytanie.quantity AS 'ilosc sztuk' FROM "
//                       "( SELECT CASE WHEN resources.craftId IS NULL THEN ingredients.name ELSE crafting.name END AS name, resources.quantity FROM "
//                       "resources LEFT JOIN crafting ON crafting.id = resources.craftId "
//                       "LEFT JOIN ingredients ON ingredients.id = resources.ingrId "

//                       "WHERE resources.userId = ";
//    question += QString::number(id);
//    question += " GROUP BY name) AS podzapytanie";

    QString question = "SELECT podzapytanie.name AS 'nazwa', podzapytanie.quantity AS 'ilosc sztuk' FROM "
                       "( SELECT CASE WHEN resources.ingrOrCraftBool = 0 THEN ingredients.name ELSE crafting.name END AS name, resources.quantity FROM "
                       "resources LEFT JOIN crafting ON crafting.id = resources.ingrOrCraftId "
                       "LEFT JOIN ingredients ON ingredients.id = resources.ingrOrCraftId "
                       "WHERE resources.userId = ";
    question += QString::number(id);
    question += " GROUP BY name) AS podzapytanie";

    SetTableView(question);
}

MainWindow::~MainWindow()
{

    delete ui;
    delete model;
    if(!logOutClicked)
        loginWindow->deleteLater();
    loginWindow = nullptr;
    db = nullptr;
    model = nullptr;
    qDebug() << "Jestem w MainWindow";
}

void MainWindow::resizeRowsInTableView()
{
    int tableWidth = ui->tableView->width()-17;
    int numColumns = model->columnCount();
    int columnWidth = tableWidth / numColumns;

    for (int col = 0; col < numColumns; ++col)
        ui->tableView->setColumnWidth(col, columnWidth);
}

void MainWindow::setUserData(QString l, int id)
{
    this->login = l;
    this->id = id;
}

void MainWindow::on_logOutButton_clicked()
{
    logOutClicked = true;
    loginWindow->show();
    this->deleteLater();
}

void MainWindow::on_actionIngrAdd_triggered()
{
    Ingredients ingr(db,id,this);
    ingr.setModal(true);
    ingr.exec();
}

void MainWindow::on_actionRecipesAdd_triggered()
{
    Recipes recipes(db,id,this);
    recipes.setModal(true);
    recipes.exec();
}


void MainWindow::on_actionResDodaj_triggered()
{
    Resources res(db,id,this);
    res.setModal(true);
    res.exec();
}

