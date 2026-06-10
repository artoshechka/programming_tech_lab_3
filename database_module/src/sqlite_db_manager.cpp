/// @file sqlite_db_manager.cpp
/// @brief Реализация методов SqliteDBManager.
/// @author Artemenko Anton

#include <database_module/src/sqlite_db.hpp>
#include <database_module/src/sqlite_db_manager.hpp>
#include <utility>

namespace database
{
namespace manager
{

SqliteDBManager::SqliteDBManager(const std::shared_ptr<logger::ILogger>& logger) : logger_(logger)
{
}

std::shared_ptr<database::IDatabase> SqliteDBManager::Create(const std::string& connectionName)
{
    return std::make_shared<SqliteDB>(connectionName, logger_);
}

}  // namespace manager
}  // namespace database
