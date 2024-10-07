#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ingredients.h"
#include "recipes.h"
#include "resources.h"
#include "removeresources.h"
#include "removerecipes.h"
#include "shoppinglist.h"
#include "search.h"
#include "account.h"
#include "rfid.h"
#include <QDebug>
#include "VHHandsetAPI.h"

// QString byteToString(const BYTE *data, int length)
// {
//     QString result;
//     for (int i = 1; i < length; ++i)
//     {
//         // Dodajemy kolejne bajty do wynikowego QStringa jako szesnastkowe wartości
//         result += QString("%1").arg(data[i], 2, 16, QChar('0')).toUpper();
//     }
//     return result;
// }

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

    userSettings = nullptr;
    loadSettings();
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
    }
    else {
        ui->addIngrToShoppingListButton->hide();
        ui->deleteIngrButton->hide();
        suggestedRecipeToPrepare = "";
    }
}

void MainWindow::connectActionTriggered()
{
    connect(ui->actionIngrAdd,&QAction::triggered, this,[=]() { Ingredients ingr(new AddIngredients(db,id),this); OpenWindow(&ingr); });
    connect(ui->actionIngrUsun,&QAction::triggered, this,[=]() { Ingredients ingr(new RemoveIngredients(db,id),this); OpenWindow(&ingr); });
    connect(ui->actionRecipesAdd,&QAction::triggered, this,[=]() { Recipes recipes(db,id,this); OpenWindow(&recipes); });
    connect(ui->actionRecUsun,&QAction::triggered, this,[=]() { RemoveRecipes removerecipes(db,id,this); OpenWindow(&removerecipes); });
    connect(ui->actionResDodaj,&QAction::triggered, this,[=]() { Resources resources(db,id,this); OpenWindow(&resources); });
    connect(ui->actionResUsun,&QAction::triggered, this,[=]() { RemoveResources removeResources(db,id,this); OpenWindow(&removeResources); });
    connect(ui->actionSettings,&QAction::triggered, this,[=]() { Settings settings(db,id,userSettings,this);
    connect(&settings,&Settings::updateSettings,this,&MainWindow::loadSettings);
    OpenWindow(&settings); });
    connect(ui->actionSzukaj,&QAction::triggered, this,[=]() { Search search(db,id,this); OpenWindow(&search); });
    connect(ui->actionPrintIngr,&QAction::triggered, this, &MainWindow::ShowIngredients);
    connect(ui->actionPrintRecipes,&QAction::triggered, this, &MainWindow::ShowRecipes);
    connect(ui->actionPrintResources,&QAction::triggered, this, &MainWindow::ShowResources);
    connect(ui->addIngrToShoppingListButton,&QPushButton::clicked, this, &MainWindow::AddIngredientsToTheShoppingList);
    connect(ui->actionPrintShoppingList,&QAction::triggered, this, &MainWindow::ShowShoppingList);
    connect(ui->actionEraseShoppingList,&QAction::triggered,this, [=]() { ingredientsOfTheRecipeMap.clear();
    namesOfAddedRecipesToShoppingListMap.clear();
    ingredientsManuallyAddedMap.clear();
    QMessageBox::information(this, "Informacja", "Wyczyszczono listę zakupów"); });
    connect(ui->actionPrintAddedRecipesToShoppingList,&QAction::triggered,this,&MainWindow::showRecipesOfTheShoppingList);
    connect(ui->actionAddIngredient,&QAction::triggered,this,[=]() { ShoppingList shoppingList(db,id,ingredientsManuallyAddedMap,this);
    connect(&shoppingList, &ShoppingList::mapEmitted, this, &MainWindow::receiveMap);
    OpenWindow(&shoppingList);});
    connect(ui->actionSaveToFile,&QAction::triggered, this, &MainWindow::saveToFile);
    connect(ui->actionReadFromFile,&QAction::triggered, this, &MainWindow::readFromFile);
    connect(ui->tableView, &QTableView::clicked, this, [this](const QModelIndex &index) {
        suggestedRecipeToPrepare = index.model()->data(index, Qt::DisplayRole).toString();
    });
    connect(ui->actionChangePassword,&QAction::triggered,this,[=]() {Account account(db,id,this); OpenWindow(&account);});

    connect(ui->actionBindRfidTag,&QAction::triggered,this,[=]() {Rfid rfid(db,id,new BindTag(db,id,true),this); OpenWindow(&rfid);});
    connect(ui->actionUnBindRfidTag,&QAction::triggered,this,[=]() {Rfid rfid(db,id,new UnBindTag(db,id,true),this); OpenWindow(&rfid);});
    connect(ui->actionStartRfid,&QAction::triggered,this,&MainWindow::prepareToReadTags);
    connect(ui->actionStopRfid,&QAction::triggered,this,&MainWindow::stopReadingTags);
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

        if (!ingredientsOfTheRecipeMap.contains(ingredient))
            ingredientsOfTheRecipeMap.insert(ingredient,quantity);
        else
            ingredientsOfTheRecipeMap[ingredient] += quantity;
    }

    if (namesOfAddedRecipesToShoppingListMap.find(suggestedRecipeToPrepare) != namesOfAddedRecipesToShoppingListMap.end())
        namesOfAddedRecipesToShoppingListMap[suggestedRecipeToPrepare] += 1;
    else
        namesOfAddedRecipesToShoppingListMap[suggestedRecipeToPrepare] = 1;
    qDebug() << suggestedRecipeToPrepare;
    QMessageBox::information(this, "Informacja", "Dodano przepis do listy zakupów");
}

void MainWindow::showRecipesOfTheShoppingList(){
    modelBezSql->clear();
    modelBezSql->setColumnCount(2);
    modelBezSql->setHeaderData(0, Qt::Horizontal, "Nazwa");
    modelBezSql->setHeaderData(1, Qt::Horizontal, "Ilość");
    int row = 0;
    for (auto it = namesOfAddedRecipesToShoppingListMap.cbegin(); it != namesOfAddedRecipesToShoppingListMap.cend(); ++it) {
        modelBezSql->setItem(row, 0, new QStandardItem(it.key()));
        modelBezSql->setItem(row, 1, new QStandardItem(QString::number(it.value())));

        // Blokowanie edycji wszystkich komórek
        modelBezSql->item(row,0)->setEditable(false);
        modelBezSql->item(row,1)->setEditable(false);

        ++row;
    }
    ui->tableView->setModel(modelBezSql);
    currentlyViewedData = CurrentlyViewedData::RecipesOfTheShoppingList;
    SuggestButtonSwitchEnable();
    resizeRowsInTableView();
}

void MainWindow::colorKcalTableView()
{
    //kopiuje wartosc z model do modelBezSql aby byly widoczne kolory w kolumnie kcal
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();

    // Ustaw liczbę wierszy i kolumn w QStandardItemModel
    modelBezSql->clear();
    modelBezSql->setRowCount(rowCount);
    modelBezSql->setColumnCount(columnCount);

    modelBezSql->setHeaderData(0, Qt::Horizontal, "Nazwa");
    modelBezSql->setHeaderData(1, Qt::Horizontal, "Ilość");
    modelBezSql->setHeaderData(2, Qt::Horizontal, "Kcal");

    // Kopiuj dane z QSqlQueryModel do QStandardItemModel
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            // Pobierz dane z QSqlQueryModel
            QModelIndex index = model->index(row, col);
            QVariant data = model->data(index);

            // Ustaw dane w QStandardItemModel
            QStandardItem *item = new QStandardItem(data.toString());
            item->setEditable(false);
            if (col==2) {
                if(data.toInt() > userSettings->maxKcal)
                    item->setData(QColor(Qt::red).lighter(75),Qt::ForegroundRole);
            }
            modelBezSql->setItem(row, col, item);
        }
    }

    ui->tableView->setModel(modelBezSql);
}

void MainWindow::saveToFile()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Zapisz plik", "", "Pliki tekstowe (*.txt)");
    if (filePath.isEmpty()) return; // Anulowano dialog

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Nie można otworzyć pliku do zapisu:" << filePath;
        return;
    }

    QTextStream out(&file);
    for (auto it = ingredientsOfTheRecipeMap.begin(); it != ingredientsOfTheRecipeMap.end(); ++it) {
        out << it.key() << ':';
        out << it.value() << '\n';
    }
    out << ";\n";
    for (auto it = namesOfAddedRecipesToShoppingListMap.begin(); it != namesOfAddedRecipesToShoppingListMap.end(); ++it) {
        out << it.key() << ':';
        out << it.value() << '\n';
    }
    out << ";\n";
    for (auto it = ingredientsManuallyAddedMap.begin(); it != ingredientsManuallyAddedMap.end(); ++it) {
        out << it.key() << ':';
        out << it.value() << '\n';
    }
    file.close();
}

void MainWindow::readFromFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Otwórz plik", "", "Pliki tekstowe (*.txt)");
    if (filePath.isEmpty()) return; // Anulowano dialog

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Nie można otworzyć pliku do odczytu:" << filePath;
        return;
    }

    QTextStream in(&file);
    int whichMapToRead = 1;
    ingredientsOfTheRecipeMap.clear();
    namesOfAddedRecipesToShoppingListMap.clear();
    ingredientsManuallyAddedMap.clear();
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line==";")
            whichMapToRead++;
        else {
            QStringList parts = line.split(':');
            if (parts.size() == 2) {
                QString key = parts.at(0);
                int value = parts.at(1).toInt();
                switch(whichMapToRead) {
                case 1: ingredientsOfTheRecipeMap.insert(key, value); break;
                case 2: namesOfAddedRecipesToShoppingListMap.insert(key, value); break;
                case 3: ingredientsManuallyAddedMap.insert(key, value); break;
                }
            }
        }
    }
    file.close();
}

void MainWindow::loadSettings()
{
    if(userSettings == nullptr){
        userSettings = new UserSettings();
        QSqlQuery query;
        query.prepare("SELECT kcal FROM users WHERE id = ?");
        query.addBindValue(id);
        query.exec();
        if(query.next()){
            userSettings->maxKcal = query.value(0).toInt();
            //userSettings->rfidBool = query.value(1).toBool();
        }

        //userSettings->rfidBool = isRfidConnected();
        userSettings->rfidBool = connectToRfid();
    }

    ui->menuRFID->setEnabled(userSettings->rfidBool);
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

            // Blokowanie edycji wszystkich komórek
            modelBezSql->item(row,0)->setEditable(false);
            modelBezSql->item(row,1)->setEditable(false);

            ++row;
        }
    }

    for (auto it = ingredientsManuallyAddedMap.cbegin(); it != ingredientsManuallyAddedMap.cend(); ++it) {
        QStandardItem *ingredient = new QStandardItem(it.key());
        ingredient->setEditable(false);
        ingredient->setData(QColor(Qt::green).lighter(75),Qt::ForegroundRole);
        QStandardItem *quantity = new QStandardItem(QString::number(it.value()));
        quantity->setEditable(false);
        quantity->setData(QColor(Qt::green).lighter(75),Qt::ForegroundRole);

        modelBezSql->setItem(row, 0, ingredient);
        modelBezSql->setItem(row, 1, quantity);

        ++row;
    }

    ui->tableView->setModel(modelBezSql);
    currentlyViewedData = CurrentlyViewedData::IngredientsOfShoppingList;
    SuggestButtonSwitchEnable();
    resizeRowsInTableView();
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

    //For RFID
    queryModelToStandardModel();
}

void MainWindow::queryModelToStandardModel()
{
    modelBezSql->clear();
    modelBezSql->setColumnCount(model->columnCount());
    for (int column = 0; column < model->columnCount(); column++){
        modelBezSql->setHeaderData(column, Qt::Horizontal, model->headerData(column, Qt::Horizontal).toString());
    }
    //modelBezSql->setHeaderData(0, Qt::Horizontal, "Nazwa");
    //modelBezSql->setHeaderData(1, Qt::Horizontal, "Ilość sztuk");

    for (int row = 0; row < model->rowCount(); ++row) {
        QList<QStandardItem*> rowItems;

        // Pobranie kolumn z sqlModel(model) i dodanie ich do rowItems
        for (int col = 0; col < model->columnCount(); ++col) {
            QString data = model->data(model->index(row, col)).toString();
            rowItems.append(new QStandardItem(data));
        }

        // Dodanie wiersza do standardModel
        modelBezSql->appendRow(rowItems);
        modelBezSql->item(row,0)->setEditable(false);
        modelBezSql->item(row,1)->setEditable(false);
    }
}

bool MainWindow::connectToRfid()
{
    VH_InitUsb(NULL);
    rfidConnection = VH_ConnectScannerUsb(&hScanner);
    if (rfidConnection != _OK)
        return false;
    return true;
}

bool MainWindow::disconnectFromRfid()
{
    if (rfidConnection == _OK)
    {
        // Odłączenie czytnika
        rfidConnection = VH_DisconnectScannerUsb(hScanner);

        // Zamknięcie połączenia USB
        rfidConnection = VH_CloseUsb();
        return true;
    }
    return false;
}

void MainWindow::prepareToReadTags()
{
    if (rfidConnection == _OK && currentlyViewedData == CurrentlyViewedData::Resources) {
        coolDownTimerRfid = new QTimer(this);
        connect(coolDownTimerRfid, &QTimer::timeout, this, &MainWindow::readTags);
        coolDownTimerRfid->start(2000);
        rfidEnabled = true;
    }
}

void MainWindow::readTags() {
    // Jezeli nie jestem w zasobach, to zatrzymaj odczyt
    if (currentlyViewedData != CurrentlyViewedData::Resources) {
        stopReadingTags();
        return;
    }

    // usun poprzednie zasoby odczytane przez rfid
    if (additionalColumnsRfid > 0) {
        modelBezSql->removeRows(0,additionalColumnsRfid);
        additionalColumnsRfid = 0;
    }
    result = EPC1G2_ReadLabelID(hScanner, 1, 0, 0, NULL, IDBuffer, &nCounterRfid, 0);
    if (result == _OK) {
        qDebug() << "Odczytano EPC NO.:";

        // Zamien byte na qstring a nastepnie rozbij ciag tagow na pojedyncze tagi
        QVector<QString> segments = extractSegments(byteToString(IDBuffer,nCounterRfid*13));

        // qDebug() << "Wyciągnięte segmenty:";
        // for (const auto& segment : segments) {
        //     qDebug() << segment;
        // }
        qDebug() << nCounterRfid;
        for (int i=0;i<nCounterRfid;i++){
            QString name = findResourceByTag(segments[i]);

            if (name != ""){
                bool foundDuplicatedName = false;
                for (int i=0;i<additionalColumnsRfid;i++){
                    if (name == modelBezSql->item(i)->text()){
                        QString amountOfResourcesQString = modelBezSql->item(i,1)->text();
                        int amountOfResourcesInt = amountOfResourcesQString.toInt() + 1;
                        modelBezSql->item(i,1)->setText(QString::number(amountOfResourcesInt));
                        foundDuplicatedName = true;
                        break;
                    }
                }

                if (foundDuplicatedName == false){
                    QStandardItem* resource = new QStandardItem(name);
                    resource->setEditable(false);
                    resource->setData(QColor(Qt::green).lighter(75),Qt::ForegroundRole);

                    //QStandardItem* item = new QStandardItem(segments[i]);
                    modelBezSql->insertRow(0, resource);
                    modelBezSql->setItem(0, 1, new QStandardItem(QString::number(1)));
                    modelBezSql->item(0,0)->setEditable(false);
                    modelBezSql->item(0,1)->setEditable(false);
                    additionalColumnsRfid++;
                }
            }
        }

        ui->tableView->setModel(modelBezSql);
    } else {
        qDebug() << "Błąd odczytu EPC NO.:" << result;
    }
}

void MainWindow::stopReadingTags()
{
    if(currentlyViewedData == CurrentlyViewedData::Resources)
        modelBezSql->removeRows(0,additionalColumnsRfid);

    rfidEnabled = false;
    nCounterRfid = 0;
    additionalColumnsRfid = 0;
    coolDownTimerRfid->stop();
    delete coolDownTimerRfid;
    coolDownTimerRfid = nullptr;
    //ShowResources();
}

QString MainWindow::findResourceByTag(const QString &tag)
{
    QString question = "SELECT podzapytanie.name AS 'nazwa' FROM "
                       "( SELECT CASE WHEN rfid.ingrOrCraftBool = 0 THEN ingredients.name ELSE crafting.name END AS name FROM "
                       "rfid LEFT JOIN crafting ON crafting.id = rfid.ingrOrCraftId "
                       "LEFT JOIN ingredients ON ingredients.id = rfid.ingrOrCraftId "
                       "WHERE rfid.userId = ? AND tagId = ? GROUP BY name) AS podzapytanie";
    QSqlQuery query;
    query.prepare(question);
    query.addBindValue(id);
    query.addBindValue(tag);
    query.exec();
    if(query.next()){
        QString name = query.value(0).toString();
        return name;
    }
    return "";
}

QVector<QString> MainWindow::extractSegments(const QString &input)
{
    QVector<QString> result;
    int n = input.size();
    int i = 0;

    while (i < n) {
        // Sprawdź, czy pozostało przynajmniej 24 znaki do wyciągnięcia
        if (i + 24 <= n) {
            result.append(input.mid(i, 24)); // Wyciągnij 24 znaki (QString)
            i += 24; // Przesuń wskaźnik o 24
        } else {
            // Jeśli mniej niż 24 znaki zostały, wyciągnij ile się da
            result.append(input.mid(i));
            break; // Koniec ciągu
        }

        // Odrzuć 2 znaki (jeśli są)
        i += 2; // Przesuń wskaźnik o kolejne 2 znaki (pomijamy je)
    }

    return result;
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
    QString question = "SELECT name AS 'nazwa', kcal FROM crafting WHERE userId IN (0,?)";
    QSqlQuery query;
    query.prepare(question);
    query.addBindValue(id);

    SetTableView(query);
    currentlyViewedData = CurrentlyViewedData::Craftings;
    SuggestButtonSwitchEnable();
}

MainWindow::~MainWindow()
{
    if(rfidEnabled)
        stopReadingTags();

    delete ui;
    delete model;
    delete modelBezSql;
    delete userSettings;
    if(!logOutClicked)
        loginWindow->deleteLater();
    if(coolDownTimerRfid) {
        delete coolDownTimerRfid;
        coolDownTimerRfid = nullptr;
    }
    loginWindow = nullptr;
    model = nullptr;
    modelBezSql = nullptr;
    userSettings = nullptr;
    disconnectFromRfid();
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
    QString question = "SELECT crafting.name, FLOOR(MIN(resources.quantity / crafting_ingredients.quantity)) AS ilość, kcal "
                       "FROM crafting "
                       "JOIN crafting_ingredients ON crafting.id = crafting_ingredients.craftingId "
                       "JOIN resources ON crafting_ingredients.ingredientId = resources.ingrOrCraftId "
                       "WHERE (resources.userId = 0 OR resources.userId = ?) "
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
                                "WHERE craftingId = crafting.id AND ( userId = ? OR userId = 0) ) "
                            "AND MIN(resources.quantity >= crafting_ingredients.quantity) = 1 ) "
                       "GROUP BY crafting.id, crafting.name;";

    QSqlQuery query;
    query.prepare(question);
    query.addBindValue(id);
    query.addBindValue(id);
    query.addBindValue(id);

    SetTableView(query);
    if (ui->tableView->model()->rowCount() == 0) {
        QMessageBox::information(this, "Informacja", "Nie znaleziono sugestii");
    }
    else {
        currentlyViewedData = CurrentlyViewedData::SuggestedCraftings;
        SuggestButtonSwitchEnable();
        colorKcalTableView();
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
    if(!suggestedRecipeToPrepare.isEmpty()) {
        int recipeId = searchCraftId(suggestedRecipeToPrepare);
        if(recipeId == -1) {
            QMessageBox::warning(this, "Błąd", "Nie można znaleźć przepisu.");
            return;
        }

        QSqlQuery queryIngredients(*db);
        queryIngredients.prepare("SELECT ingredientId, quantity FROM crafting_ingredients WHERE craftingId = ?");
        queryIngredients.addBindValue(recipeId);

        if(!queryIngredients.exec()) {
            QMessageBox::critical(this, "Błąd", "Błąd podczas szukania składników przepisu.");
            return;
        }

        db->transaction();
        bool errorOccurred = false;

        while(queryIngredients.next()) {
            int ingredientId = queryIngredients.value(0).toInt();
            int quantityToRemove = queryIngredients.value(1).toInt();

            int currentQuantity = getResourceQuantity(ingredientId, true);
            if(currentQuantity > 0) {
                if(!updateResourceQuantity(ingredientId, -quantityToRemove, true)) {
                    errorOccurred = true;
                    QMessageBox::warning(this, "Błąd", "Nie udało się usunąć składnika o ID " + QString::number(ingredientId) + " z zasobów.");
                    break;
                }
            }
        }

        if(errorOccurred) {
            db->rollback();
            QMessageBox::critical(this, "Błąd", "Wystąpił problem podczas usuwania składników z zasobów.");
        } else {
            db->commit();
            QMessageBox::information(this, "Sukces", "Składniki przepisu zostały usunięte z zasobów.");
        }
    }
    resizeRowsInTableView();
    ShowResources();
}

