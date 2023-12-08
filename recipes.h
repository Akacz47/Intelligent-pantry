#ifndef RECIPES_H
#define RECIPES_H

#include "baseclass.h"

namespace Ui {
class Recipes;
}

class Recipes : public BaseClass
{
    Q_OBJECT

public:
    explicit Recipes(QSqlDatabase* db, int id, QWidget *parent = nullptr);
    ~Recipes();

private:
    Ui::Recipes *ui;
};

#endif // RECIPES_H
