#include "database.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

QString Database::resourcesQuery()
{
    return QStringLiteral(
        "SELECT CASE WHEN r.ingrOrCraftBool = 0 THEN i.name ELSE c.name END AS nazwa, "
        "r.quantity AS ilość "
        "FROM resources r "
        "LEFT JOIN ingredients i ON r.ingrOrCraftBool = 0 AND i.id = r.ingrOrCraftId "
        "LEFT JOIN crafting c ON r.ingrOrCraftBool = 1 AND c.id = r.ingrOrCraftId "
        "WHERE r.userId = ? "
        "ORDER BY nazwa COLLATE NOCASE, r.ingrOrCraftBool, r.id");
}

bool Database::open(QSqlDatabase &database, QString &error, const QString &path)
{
    error.clear();
    QString filePath = path;
    if (filePath.isEmpty())
        filePath = qEnvironmentVariable("SPIZARKA_DB_PATH");
    if (filePath.isEmpty()) {
        const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        if (directory.isEmpty()) {
            error = QStringLiteral("Nie znaleziono katalogu na bazę danych.");
            return false;
        }
        filePath = directory + "/spizarka.sqlite";
    }
    if (filePath != ":memory:" && !QDir().mkpath(QFileInfo(filePath).absolutePath())) {
        error = QStringLiteral("Nie można utworzyć katalogu bazy danych: %1").arg(filePath);
        return false;
    }

    database.setDatabaseName(filePath);
    database.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000");
    if (!database.open()) {
        error = database.lastError().text();
        return false;
    }

    const auto fail = [&](const QString &message) {
        error = message;
        database.rollback();
        database.close();
        return false;
    };
    QSqlQuery query(database);
    if (!query.exec("PRAGMA foreign_keys = ON"))
        return fail(query.lastError().text());
    if (!query.exec("PRAGMA user_version") || !query.next())
        return fail(query.lastError().text());
    const int version = query.value(0).toInt();
    query.finish();
    if (version > 2)
        return fail(QStringLiteral("Baza danych pochodzi z nowszej wersji aplikacji."));
    if (version == 2)
        return true;

    if (!database.transaction())
        return fail(database.lastError().text());
    const QStringList migrations = version == 0
        ? QStringList{":/database/schema.sql", ":/database/seed.sql"}
        : QStringList{":/database/seed.sql"};
    for (const QString &migration : migrations) {
        QFile script(migration);
        if (!script.open(QIODevice::ReadOnly | QIODevice::Text))
            return fail(script.errorString());
        const QStringList statements = QString::fromUtf8(script.readAll()).split(';', Qt::SkipEmptyParts);
        for (const QString &statement : statements) {
            if (!statement.trimmed().isEmpty() && !query.exec(statement))
                return fail(query.lastError().text());
        }
    }
    if (!query.exec("PRAGMA user_version = 2"))
        return fail(query.lastError().text());
    if (!database.commit())
        return fail(database.lastError().text());
    return true;
}
