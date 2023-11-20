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
}

MainWindow::~MainWindow()
{
    delete ui;
    delete loginWindow;
    loginWindow = nullptr;
}
