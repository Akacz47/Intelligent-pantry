#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ingredients.h"
#include "recipes.h"
#include "resources.h"
#include "removeresources.h"

MainWindow::MainWindow(QString l, int id, QSqlDatabase* db, QDialog *parent) :
    QMainWindow(parent),
    BaseClass(db,id),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->login = l;
    loginWindow = parent;
    setAttribute(Qt::WA_DeleteOnClose);
    ui->welcomeLabel->setText("Witaj " + this->login);
    setWindowTitle("Spizarka");
    logOutClicked = false;
    model = new QSqlQueryModel();
    modelBezSql = new QStandardItemModel();
    connectActionTriggered();
    ShowResources();
    suggestedRecipeToPrepare = "";
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    resizeRowsInTableView();
}

void MainWindow::SetTableView(QSqlQuery& query)
{
    query.exec();
    model->setQuery(std::move(query));
    ui->tableView->setModel(model);
    resizeRowsInTableView();
}

void MainWindow::SuggestButtonSwitchEnable()
{
    if (currentlyViewedData == CurrentlyViewedData::SuggestedCraftings){
        ui->deleteIngrButton->show();
        ui->addIngrToShoppingListButton->hide();
    }
    else if(currentlyViewedData == CurrentlyViewedData::IngredientsOfTheRecipe) {
        ui->addIngrToShoppingListButton->show();
        ui->deleteIngrButton->hide();
        suggestedRecipeToPrepare = "";
    }
    else {
        ui->addIngrToShoppingListButton->hide();
        ui->deleteIngrButton->hide();
        suggestedRecipeToPrepare = "";
    }
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
    connect(ui->addIngrToShoppingListButton,&QPushButton::clicked, this, &MainWindow::AddIngredientsToTheShoppingList);
    connect(ui->actionPrintShoppingList,&QAction::triggered, this, &MainWindow::ShowShoppingList);
    connect(ui->actionEraseShoppingList,&QAction::triggered,this, [=]() { ingredientsOfTheRecipeMap.clear();
    QMessageBox::information(this, "Informacja", "Wyczyszczono listę zakupów"); });
}

void MainWindow::ShowIngredientsOfRecipe(QString name)
{
    int craftingId = searchCraftId(name);
    QString question = "SELECT i.name AS nazwa, ci.quantity AS ilosc FROM ingredients AS i "
                       "JOIN crafting_ingredients AS ci ON i.id = ci.ingredientId "
                       "WHERE ci.craftingId = ? AND ci.userId IN(0,?);";
    QSqlQuery query;
    query.prepare(question);
    query.addBindValue(craftingId);
    query.addBindValue(id);

    SetTableView(query);
    currentlyViewedData = CurrentlyViewedData::IngredientsOfTheRecipe;
    SuggestButtonSwitchEnable();
}

void MainWindow::AddIngredientsToTheShoppingList()
{
    for (int i=0; i<model->rowCount();i++) {
        //sprawdz czy istnieje i dodaj skladniki
        QModelIndex ingredientIndex = model->index(i,0);
        QString ingredient = model->data(ingredientIndex).toString();

        QModelIndex quantityIndex = model->index(i,1);
        int quantity = model->data(quantityIndex).toInt();
        if (ingredientsOfTheRecipeMap.find(ingredient) != ingredientsOfTheRecipeMap.end())
            ingredientsOfTheRecipeMap[ingredient] += quantity;
        else
            ingredientsOfTheRecipeMap[ingredient] = quantity;
    }
    QMessageBox::information(this, "Informacja", "Dodano skladniki do listy zakupów");
}

void MainWindow::ShowShoppingList()
{
    modelBezSql->clear();
    modelBezSql->setColumnCount(2);
    modelBezSql->setHeaderData(0, Qt::Horizontal, "Nazwa");
    modelBezSql->setHeaderData(1, Qt::Horizontal, "Ilość");
    int row = 0;
    for (auto it = ingredientsOfTheRecipeMap.cbegin(); it != ingredientsOfTheRecipeMap.cend(); ++it) {
        int quantityInResources = searchResourceQuantity(it.key());

        //minus ilosc skladnika w zasobach
        int quantityToPrint = it.value() - quantityInResources;

        if (quantityToPrint > 0 ){
            modelBezSql->setItem(row, 0, new QStandardItem(it.key()));
            modelBezSql->setItem(row, 1, new QStandardItem(QString::number(quantityToPrint)));
            ++row;
        }
    }

    // Blokowanie edycji wszystkich komórek
    for (int row = 0; row < modelBezSql->rowCount(); ++row) {
        for (int col = 0; col < modelBezSql->columnCount(); ++col) {
            modelBezSql->item(row, col)->setEditable(false);
        }
    }

    ui->tableView->setModel(modelBezSql);
    currentlyViewedData = CurrentlyViewedData::IngredientsOfShoppingList;
    SuggestButtonSwitchEnable();
}

void MainWindow::OpenWindow(QDialog* reference)
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
                       "WHERE resources.userId = ? GROUP BY name) AS podzapytanie";
    QSqlQuery query;
    query.prepare(question);
    query.addBindValue(id);

    SetTableView(query);
    currentlyViewedData = CurrentlyViewedData::Resources;
    SuggestButtonSwitchEnable();
}

void MainWindow::ShowIngredients()
{
    QString question = "SELECT name AS 'nazwa' FROM ingredients WHERE userId IN (0,?)";
    QSqlQuery query;
    query.prepare(question);
    query.addBindValue(id);

    SetTableView(query);
    currentlyViewedData = CurrentlyViewedData::Ingredients;
    SuggestButtonSwitchEnable();
}

void MainWindow::ShowRecipes()
{
    QString question = "SELECT name AS 'nazwa' FROM crafting WHERE userId IN (0,?)";
    QSqlQuery query;
    query.prepare(question);
    query.addBindValue(id);

    SetTableView(query);
    currentlyViewedData = CurrentlyViewedData::Craftings;
    SuggestButtonSwitchEnable();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
    delete modelBezSql;
    if(!logOutClicked)
        loginWindow->deleteLater();
    loginWindow = nullptr;
    model = nullptr;
    modelBezSql = nullptr;
    qDebug() << "Jestem w MainWindow";
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
                       "WHERE resources.userId = 0 OR resources.userId = ? "
                       "AND resources.ingrOrCraftBool = 0 "
                       "AND crafting.id IN ( "
                            "SELECT crafting.id "
                            "FROM crafting "
                            "JOIN crafting_ingredients ON crafting.id = crafting_ingredients.craftingId "
                            "JOIN resources ON crafting_ingredients.ingredientId = resources.ingrOrCraftId "
                            "WHERE (resources.userId = 0 OR resources.userId = ?) "
                            "AND resources.ingrOrCraftBool = 0 "
                            "GROUP BY crafting.id "
                            "HAVING COUNT(DISTINCT crafting_ingredients.ingredientId) = ( "
                                "SELECT COUNT(*) "
                                "FROM crafting_ingredients "
                                "WHERE craftingId = crafting.id ) "
                            "AND MIN(resources.quantity >= crafting_ingredients.quantity) = 1 ) "
                       "GROUP BY crafting.id, crafting.name;";

    QSqlQuery query;
    query.prepare(question);
    query.addBindValue(id);
    query.addBindValue(id);

    SetTableView(query);
    if (ui->tableView->model()->rowCount() == 0) {
        QMessageBox::information(this, "Informacja", "Nie znaleziono sugestii");
    }
    else {
        currentlyViewedData = CurrentlyViewedData::SuggestedCraftings;
        SuggestButtonSwitchEnable();
    }
}

void MainWindow::resizeRowsInTableView()
{
    int tableWidth = ui->tableView->width()-17;
    int numColumns = model->columnCount();
    int columnWidth = tableWidth / numColumns;

    for (int col = 0; col < numColumns; ++col)
        ui->tableView->setColumnWidth(col, columnWidth);
}

void MainWindow::on_tableView_activated(const QModelIndex &index)
{
    QString name = index.model()->data(index.model()->index(index.row(), 0)).toString();
    if (currentlyViewedData == CurrentlyViewedData::Craftings || currentlyViewedData == CurrentlyViewedData::SuggestedCraftings) {
        suggestedRecipeToPrepare = name;
        ShowIngredientsOfRecipe(name);
    }
}


void MainWindow::on_deleteIngrButton_clicked()
{
    if(suggestedRecipeToPrepare != ""){

    }
}

