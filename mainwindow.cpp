#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ingredients.h"
#include "recipes.h"
#include "resources.h"
#include "removeresources.h"

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
    currentlyViewedData = CurrentlyViewedData::Resources;
    suggestedRecipeToPrepare = "";

    connectActionTriggered();
}

void MainWindow::SetTableView(const QString& str)
{
    model->setQuery(std::move(str));
    ui->tableView->setModel(model);
    resizeRowsInTableView();
}

void MainWindow::SuggestButtonSwitchEnable()
{
    if (currentlyViewedData == CurrentlyViewedData::SuggestedCraftings){
        ui->deleteIngrButton->show();
    }else{
        ui->deleteIngrButton->hide();
    }
    suggestedRecipeToPrepare = "";
}

void MainWindow::connectActionTriggered()
{
    connect(ui->actionIngrAdd,&QAction::triggered, this,[=]() { Ingredients ingr(db,id,this); OpenWindow(&ingr); });
    connect(ui->actionRecipesAdd,&QAction::triggered, this,[=]() { Recipes recipes(db,id,this); OpenWindow(&recipes); });
    connect(ui->actionResDodaj,&QAction::triggered, this,[=]() { Resources resources(db,id,this); OpenWindow(&resources); });
    connect(ui->actionResUsun,&QAction::triggered, this,[=]() { RemoveResources removeResources(db,id,this); OpenWindow(&removeResources); });
    connect(ui->actionPrintIngr,&QAction::triggered, this, &MainWindow::ShowIngredients);
    connect(ui->actionPrintRecipes,&QAction::triggered, this, &MainWindow::ShowRecipes);
    connect(ui->actionPrintResources,&QAction::triggered, this, &MainWindow::ShowResources);
}

void MainWindow::OpenWindow(BaseClass* reference)
{
    reference->setModal(true);
    reference->exec();
}

void MainWindow::ShowResources()
{
    //zapytanie ktore dzialalo, gdy w tabeli sa null'e
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
    currentlyViewedData = CurrentlyViewedData::Resources;
    SuggestButtonSwitchEnable();
}

void MainWindow::ShowIngredients()
{
    QString question = "SELECT name AS 'nazwa' FROM ingredients WHERE userId = 0 OR userId = ";
    question += QString::number(id);
    SetTableView(question);
    currentlyViewedData = CurrentlyViewedData::Ingredients;
    SuggestButtonSwitchEnable();
}

void MainWindow::ShowRecipes()
{
    QString question = "SELECT name AS 'nazwa' FROM crafting WHERE userId = 0 OR userId = ";
    question += QString::number(id);
    SetTableView(question);
    currentlyViewedData = CurrentlyViewedData::Craftings;
    SuggestButtonSwitchEnable();
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
    this->close();
}

void MainWindow::on_suggetButton_clicked()
{
    QString question = "SELECT crafting.name, FLOOR(MIN(resources.quantity / crafting_ingredients.quantity)) AS ilość "
                       "FROM crafting "
                       "JOIN crafting_ingredients ON crafting.id = crafting_ingredients.craftingId "
                       "JOIN resources ON crafting_ingredients.ingredientId = resources.ingrOrCraftId "
                       "WHERE resources.userId = 0 OR resources.userId = ";
                       question += QString::number(id);
                       question += " AND resources.ingrOrCraftBool = 0 "
                       "AND crafting.id IN ( "
                            "SELECT crafting.id "
                            "FROM crafting "
                            "JOIN crafting_ingredients ON crafting.id = crafting_ingredients.craftingId "
                            "JOIN resources ON crafting_ingredients.ingredientId = resources.ingrOrCraftId "
                            "WHERE (resources.userId = 0 OR resources.userId = ";
                            question += QString::number(id);
                            question += ") AND resources.ingrOrCraftBool = 0 "
                            "GROUP BY crafting.id "
                            "HAVING COUNT(DISTINCT crafting_ingredients.ingredientId) = ( "
                                "SELECT COUNT(*) "
                                "FROM crafting_ingredients "
                                "WHERE craftingId = crafting.id ) "
                            "AND MIN(resources.quantity >= crafting_ingredients.quantity) = 1 ) "
                       "GROUP BY crafting.id, crafting.name;";

    SetTableView(question);
    if (ui->tableView->model()->rowCount() == 0) {
        QMessageBox::information(this, "Informacja", "Nie znaleziono sugestii");
    }
    else {
        currentlyViewedData = CurrentlyViewedData::SuggestedCraftings;
        SuggestButtonSwitchEnable();
    }
}


void MainWindow::on_tableView_activated(const QModelIndex &index)
{
    if(currentlyViewedData == CurrentlyViewedData::SuggestedCraftings) {
        suggestedRecipeToPrepare = index.model()->data(index.model()->index(index.row(), 0)).toString();
    }
}


void MainWindow::on_deleteIngrButton_clicked()
{
    if(suggestedRecipeToPrepare != ""){

    }
}

