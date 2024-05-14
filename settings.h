#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "baseclass.h"


namespace Ui {
class Settings;
}

struct UserSettings {
    int maxKcal;
    bool rfidBool;
};

class Settings : public QDialog, public BaseClass
{
    Q_OBJECT
    UserSettings* us;
public:
    explicit Settings(QSqlDatabase* db, int id, UserSettings* us, QWidget *parent = nullptr);
    ~Settings();

private slots:
    void on_okButton_clicked();

signals:
    void updateSettings();

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
