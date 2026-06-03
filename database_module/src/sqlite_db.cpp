/// @file sqlite_db.cpp
/// @brief Определение класса SqliteDB
/// @author Artemenko Anton

#include <database_module/src/sqlite_db.hpp>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QString>

namespace database
{

SqliteDB::SqliteDB(const std::string& connectionName)
    : db_(QSqlDatabase::addDatabase("QSQLITE", QString::fromStdString(connectionName)))
{
}

SqliteDB::~SqliteDB()
{
    const QString name = db_.connectionName();
    db_.close();
    QSqlDatabase::removeDatabase(name);
}

bool SqliteDB::Open(const std::string& path)
{
    // NUL обрывает строку в C-API SQLite: путь "ok.db\0../secret" открыл бы не тот файл.
    if (path.find('\0') != std::string::npos)
        return false;
    db_.setDatabaseName(QString::fromStdString(path));
    return db_.open();
}

void SqliteDB::Close()
{
    db_.close();
}

std::vector<std::string> SqliteDB::Tables() const
{
    std::vector<std::string> result;
    for (const QString& t : db_.tables())
        result.push_back(t.toStdString());
    return result;
}

void SqliteDB::Query(const std::string& sql,
                     std::function<void(const std::string& col, const std::string& val)> rowFn)
{
    QSqlQuery query(db_);
    if (!query.exec(QString::fromStdString(sql)))
        return;
    const QSqlRecord rec = query.record();
    while (query.next())
    {
        for (int i = 0; i < rec.count(); ++i)
            rowFn(rec.fieldName(i).toStdString(), query.value(i).toString().toStdString());
    }
}

}  // namespace database
