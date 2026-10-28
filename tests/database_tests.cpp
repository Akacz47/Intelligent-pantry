#include "database.h"
#include "baseclass.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QFileInfo>
#include <QMap>
#include <QSqlError>
#include <QSqlQuery>
#include <QTemporaryDir>
#include <cstdlib>

static void require(bool condition, const QString &message)
{
    if (!condition)
        qFatal("%s", qPrintable(message));
}

static void execute(QSqlDatabase &db, const QString &sql)
{
    QSqlQuery query(db);
    require(query.exec(sql), sql + ": " + query.lastError().text());
}

static QVariant scalar(QSqlDatabase &db, const QString &sql)
{
    QSqlQuery query(db);
    require(query.exec(sql) && query.next(), sql + ": " + query.lastError().text());
    return query.value(0);
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QTemporaryDir directory;
    require(directory.isValid(), "Temporary directory");
    const QString path = directory.filePath("nested/data/pantry.sqlite");
    require(!QFileInfo::exists(path), "Database initially absent");
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    QString error;
    require(Database::open(db, error, path), error);
    require(QFileInfo::exists(path), "Missing directory and database created automatically");
    require(db.tables().contains("rfid") && db.tables().contains("crafting_ingredients"), "Schema created");
    require(scalar(db, "PRAGMA foreign_keys").toInt() == 1, "Foreign keys enabled");
    require(scalar(db, "SELECT COUNT(*) FROM ingredients WHERE userId=0").toInt() == 20, "Shared ingredients seeded");
    require(scalar(db, "SELECT COUNT(*) FROM crafting WHERE userId=0").toInt() == 6, "Shared recipes seeded");
    require(scalar(db, "SELECT COUNT(*) FROM crafting_ingredients WHERE userId=0").toInt() == 22, "Recipe portions seeded");
    require(scalar(db, "SELECT COUNT(*) FROM users").toInt() == 0, "No demo accounts created");
    execute(db, "DELETE FROM crafting_ingredients");
    execute(db, "DELETE FROM crafting");
    execute(db, "DELETE FROM ingredients");
    execute(db, "DELETE FROM sqlite_sequence WHERE name IN ('ingredients','crafting')");

    const auto hash = [](const QString &text) {
        return QString::fromLatin1(QCryptographicHash::hash((text + "twsRmlJermsjpgN").toUtf8(), QCryptographicHash::Sha256).toHex());
    };
    const QString login = hash("sqlite-test");
    const QString password = hash("TestPassword123!");
    {
        QSqlQuery user(db);
        user.prepare("INSERT INTO users (login, oldPassword, currentPassword, kcal) VALUES (?, ?, ?, ?)");
        user.addBindValue(login);
        user.addBindValue(password);
        user.addBindValue(password);
        user.addBindValue(2000);
        require(user.exec(), user.lastError().text());
        require(user.lastInsertId().toInt() == 1, "Automatic user ID");
    }
    require(scalar(db, "SELECT rfidBool FROM users WHERE id=1").toInt() == 0, "RFID defaults off");
    require(scalar(db, "SELECT currentPassword FROM users WHERE login='" + login + "'").toString() == password, "Authentication data");
    execute(db, "INSERT INTO users (login, oldPassword, currentPassword) VALUES ('second', 'hash', 'hash')");
    execute(db, "INSERT INTO ingredients (name, unit, userId) VALUES ('Mąka', 'g', 1), ('Woda', 'ml', 0)");
    execute(db, "INSERT INTO crafting (name, kcal, userId) VALUES ('Chleb', 500, 1)");
    execute(db, "INSERT INTO crafting_ingredients (ingredientId, craftingId, quantity, userId) VALUES (1,1,200,1), (2,1,100,1)");
    execute(db, "INSERT INTO resources (quantity, userId, ingrOrCraftId, ingrOrCraftBool) VALUES (550,1,1,0), (300,1,2,0), (20,2,1,0), (1,1,1,1)");
    execute(db, "INSERT INTO rfid (tagId, ingrOrCraftId, ingrOrCraftBool, userId) VALUES ('0000ABCDEF1234567890',1,0,1), ('00000000000000000001',1,1,1)");
    require(scalar(db, "SELECT tagId FROM rfid WHERE id=1").toString() == "0000ABCDEF1234567890", "RFID leading zeros");

    {
        QSqlQuery stock(db);
        require(stock.prepare(Database::resourcesQuery()), stock.lastError().text());
        stock.addBindValue(1);
        require(stock.exec(), stock.lastError().text());
        QMap<QString, int> displayed;
        int rows = 0;
        while (stock.next()) {
            displayed.insert(stock.value(0).toString(), stock.value(1).toInt());
            ++rows;
        }
        require(rows == 3 && displayed.value("Mąka") == 550 && displayed.value("Woda") == 300
                && displayed.value("Chleb") == 1, "Stock view resolves overlapping ingredient/recipe IDs and excludes other users");
        stock.finish();

        execute(db, "INSERT INTO crafting (id,name,kcal,userId) VALUES (999,'Mąka',100,1)");
        execute(db, "INSERT INTO resources (quantity,userId,ingrOrCraftId,ingrOrCraftBool) VALUES (7,1,999,1)");
        stock.bindValue(0, 1);
        require(stock.exec(), stock.lastError().text());
        int sameNameCount = 0;
        while (stock.next())
            if (stock.value(0).toString() == "Mąka")
                ++sameNameCount;
        require(sameNameCount == 2, "Stock view retains ingredient and recipe with the same name");
        stock.finish();
        execute(db, "DELETE FROM resources WHERE ingrOrCraftId=999 AND ingrOrCraftBool=1");
        execute(db, "DELETE FROM crafting WHERE id=999");
        stock.bindValue(0, 999);
        require(stock.exec() && !stock.next(), "Empty stock returns no rows without an SQL error");
    }

    BaseClass pantry(&db, 1);
    require(pantry.getIngredientId("Mąka") == 1 && pantry.getIngredientId("Woda") == 2, "Private/shared ingredients");
    require(pantry.getResourceQuantity(1, true) == 550, "Read ingredient stock");
    require(pantry.getResourceQuantity(1, false) == 1, "Read prepared recipe stock");
    require(pantry.updateResourceQuantity(1, -50, true), "Update stock");
    require(scalar(db, "SELECT quantity FROM resources WHERE userId=2").toInt() == 20, "Other user's stock unchanged");
    require(scalar(db, "SELECT CAST(MIN(1.0 * r.quantity / ci.quantity) AS INTEGER) FROM crafting_ingredients ci JOIN resources r ON r.ingrOrCraftId=ci.ingredientId WHERE r.userId=1 AND r.ingrOrCraftBool=0 AND ci.craftingId=1").toInt() == 2, "Whole recipe portions");
    {
        QSqlQuery invalid(db);
        require(!invalid.exec("INSERT INTO crafting_ingredients (ingredientId,craftingId,quantity,userId) VALUES (999,1,1,1)"), "Missing ingredient rejected");
        require(!invalid.exec("INSERT INTO users (login,oldPassword,currentPassword) VALUES ('second','hash','hash')"), "Duplicate login rejected");
    }
    execute(db, "UPDATE users SET kcal=2400, rfidBool=1, oldPassword=currentPassword, currentPassword='new-hash' WHERE id=1");
    require(db.transaction(), "Begin transaction");
    execute(db, "INSERT INTO crafting (name,kcal,userId) VALUES ('Rolled back', 10, 1)");
    require(db.rollback(), "Rollback transaction");
    require(scalar(db, "SELECT COUNT(*) FROM crafting").toInt() == 1, "Rollback leaves no partial recipe");

    // Simulate a version-1 database containing private data and no demo data.
    execute(db, "PRAGMA user_version=1");
    db.close();
    require(Database::open(db, error, path), error);
    require(scalar(db, "SELECT COUNT(*) FROM crafting WHERE userId=0").toInt() == 6, "Existing database receives recipes");
    require(scalar(db, "SELECT COUNT(*) FROM crafting WHERE userId=1").toInt() == 1, "Migration preserves private recipe");
    const int ingredientCount = scalar(db, "SELECT COUNT(*) FROM ingredients").toInt();
    db.close();
    require(Database::open(db, error, path), error);
    require(scalar(db, "SELECT COUNT(*) FROM ingredients").toInt() == ingredientCount, "Reopening does not duplicate seeds");
    require(scalar(db, "SELECT kcal FROM users WHERE id=1").toInt() == 2400, "Settings persist after reopening");
    require(scalar(db, "SELECT currentPassword FROM users WHERE id=1").toString() == "new-hash", "Password persists");
    require(pantry.getResourceQuantity(1, true) == 500, "Stock persists");
    require(scalar(db, "SELECT COUNT(*) FROM rfid").toInt() == 2, "Tags persist");
    require(pantry.updateResourceQuantity(1, -1, false), "Remove prepared recipe stock");
    execute(db, "DELETE FROM rfid WHERE ingrOrCraftBool=1");
    require(pantry.deleteRecipe(1), "Delete recipe and its ingredients");
    require(scalar(db, "SELECT COUNT(*) FROM crafting_ingredients WHERE userId=1").toInt() == 0, "Private recipe links deleted");
    require(scalar(db, "SELECT COUNT(*) FROM crafting WHERE userId=0").toInt() == 6, "Shared recipes preserved");
    execute(db, "PRAGMA user_version=3");
    db.close();
    require(!Database::open(db, error, path) && !error.isEmpty(), "Future schema version rejected");
    require(!Database::open(db, error, directory.path()), "Invalid database path rejected");
    db.close();
    qInfo() << "SQLite integration tests passed";
    return EXIT_SUCCESS;
}
