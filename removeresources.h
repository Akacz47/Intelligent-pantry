#ifndef REMOVERESOURCES_H
#define REMOVERESOURCES_H

#include <QDialog>
#include <QMessageBox>
#include "baseclass.h"
#include "resources.h"

namespace Ui {
class RemoveResources;
}

class RemoveResources : public BaseClass
{
    Q_OBJECT

public:
    explicit RemoveResources(QSqlDatabase* db, int id, QWidget *parent = nullptr);
    ~RemoveResources();
    void removeResource(int resourceId, int quantity);
    void removeAllResource(int resourceId);
public slots:
    void on_removeAllButton_clicked();
    void on_okButton_clicked();
private:
    Ui::RemoveResources *ui;
    Resources* resourcesHelper;
signals:
    void resourcesUpdated();
};

#endif // REMOVERESOURCES_H
