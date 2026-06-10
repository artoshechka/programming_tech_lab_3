/// @file sqlite_db.cpp
/// @brief Реализация методов SqliteDB.
/// @author Artemenko Anton

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QVariant>
#include <database_module/src/sqlite_db.hpp>
#include <logger/logger_macros.hpp>
#include <stdexcept>
#include <utility>

namespace database
{

SqliteDB::SqliteDB(const std::string& connectionName, std::shared_ptr<logger::ILogger> logger)
    : db_(QSqlDatabase::addDatabase("QSQLITE", QString::fromStdString(connectionName))), logger_(std::move(logger))
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
    {
        LogError(logger_) << "SQLite open rejected: embedded NUL in path";
        return false;
    }
    db_.setDatabaseName(QString::fromStdString(path));
    if (!db_.open())
    {
        LogError(logger_) << "SQLite open failed: " << path << ": " << db_.lastError().text().toStdString();
        return false;
    }
    LogDebug(logger_) << "SQLite opened: " << path;
    return true;
}

void SqliteDB::Close()
{
    db_.close();
}

std::vector<std::string> SqliteDB::Tables() const
{
    std::vector<std::string> result;
    for (const QString& t : db_.tables()) result.push_back(t.toStdString());
    return result;
}

void SqliteDB::Query(const std::string& sql, std::function<void(const std::string& col, const std::string& val)> rowFn)
{
    QSqlQuery query(db_);
    if (!query.exec(QString::fromStdString(sql)))
    {
        const std::string err = query.lastError().text().toStdString();
        LogError(logger_) << "SQLite query failed: " << err;
        throw std::runtime_error("SQLite query failed: " + err);
    }
    const QSqlRecord rec = query.record();
    while (query.next())
    {
        for (int i = 0; i < rec.count(); ++i)
            rowFn(rec.fieldName(i).toStdString(), query.value(i).toString().toStdString());
    }
}

}  // namespace database
