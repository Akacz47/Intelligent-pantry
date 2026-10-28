#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>

namespace Database {
// An empty path uses the local application data directory.
bool open(QSqlDatabase &database, QString &error, const QString &path = {});
// Bind the current user's ID before executing.
QString resourcesQuery();
}

#endif
