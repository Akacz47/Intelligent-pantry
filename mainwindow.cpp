#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString l, int id, QSqlDatabase* db, QDialog *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->login = l;
    this->id = id;
    this->db = db;
    loginWindow = parent;
    setAttribute(Qt::WA_DeleteOnClose);
    ui->welcomeLabel->setText("Witaj " + this->login);
    setWindowTitle("Spizarka");

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
    QString question = "SELECT podzapytanie.name AS 'nazwa', podzapytanie.quantity AS 'ilosc sztuk' FROM "
                       "( SELECT CASE WHEN resources.craftId IS NULL THEN ingredients.name ELSE crafting.name END AS name, resources.quantity FROM "
                       "resources LEFT JOIN crafting ON crafting.id = resources.craftId "
                       "LEFT JOIN ingredients ON ingredients.id = resources.ingrId "

                       "WHERE resources.userId = ";
    question += QString::number(id);
    question += " GROUP BY name) AS podzapytanie";
    SetTableView(question);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete loginWindow;
    loginWindow = nullptr;
}

void MainWindow::resizeRowsInTableView()
{
    int tableWidth = ui->tableView->width()-17;
    int numColumns = model->columnCount();
    int columnWidth = tableWidth / numColumns;

    for (int col = 0; col < numColumns; ++col)
        ui->tableView->setColumnWidth(col, columnWidth);
}

