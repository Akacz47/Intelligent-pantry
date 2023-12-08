#include "recipes.h"
#include "ui_recipes.h"

Recipes::Recipes(QSqlDatabase* db, int id, QWidget *parent) :
    BaseClass(db,id,parent),
    ui(new Ui::Recipes)
{
    ui->setupUi(this);
}

Recipes::~Recipes()
{
    delete ui;
}
