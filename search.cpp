#include "search.h"
#include "qsqlerror.h"
#include "ui_search.h"
#include <QDebug>

Search::Search(QSqlDatabase* db, int id, QWidget *parent) :
    QDialog(parent),
    BaseClass(db, id),
    ui(new Ui::Search)
{
    ui->setupUi(this);
    ui->defaultRadioButton->setChecked(true);
    ui->allRadioButton_3->setChecked(true);
    ui->searchingTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

Search::~Search()
{
    delete ui;
}

void Search::on_searchButton_clicked() {
    QString searchText = ui->searchLineEdit->text().trimmed();
    if (searchText.isEmpty() || (!ui->allRadioButton_3->isChecked() && !ui->ingrRadioButton_4->isChecked() && !ui->recRadioButton_5->isChecked())) {
        QMessageBox::warning(this, "Brak danych", "Proszę wprowadzić tekst do wyszukania i wybrać filtr.");
        return;
    }

    QString queryStr;
    bool isQuerySet = false;

    if (ui->allRadioButton_3->isChecked()) {
        queryStr = QString(
            "SELECT 'Ingredient' as Type, name FROM ingredients WHERE userId IN (0, %1) AND name LIKE '%%2%' "
            "UNION "
            "SELECT 'Recipe' as Type, name FROM crafting WHERE userId IN (0, %1) AND name LIKE '%%2%'")
            .arg(this->id).arg(searchText);
        isQuerySet = true;
    } else if (ui->ingrRadioButton_4->isChecked()) {
        queryStr = QString(
            "SELECT 'Ingredient' as Type, name FROM ingredients WHERE userId IN (0, %1) AND name LIKE '%%2%'")
            .arg(this->id).arg(searchText);
        isQuerySet = true;
    } else if (ui->recRadioButton_5->isChecked()) {
        queryStr = QString(
            "SELECT 'Recipe' as Type, name FROM crafting WHERE userId IN (0, %1) AND name LIKE '%%2%'")
            .arg(this->id).arg(searchText);
        isQuerySet = true;
    }

    if (isQuerySet) {
        if (ui->alphRadioButton->isChecked()) {
            queryStr += " ORDER BY name ASC";
        } else if (ui->alphRevRadioButton_2->isChecked()) {
            queryStr += " ORDER BY name DESC";
        }
    }

    if (!isQuerySet) {
        qDebug() << "Zapytanie nie zostało ustawione.";
        return;
    }

    QSqlQuery query(*db);
    if (!query.exec(queryStr)) {
        qDebug() << "Błąd zapytania wyszukiwania: " << query.lastError();
    } else {
        QSqlQueryModel *model = new QSqlQueryModel();
        model->setQuery(std::move(query));
        model->setHeaderData(0, Qt::Horizontal, QObject::tr("Typ"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("Nazwa"));
        ui->searchingTableView->setModel(model);
    }

}
