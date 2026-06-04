/// @file sqlite_db_manager.cpp
/// @brief Определение класса SqliteDBManager
/// @author Artemenko Anton

#include <database_module/src/sqlite_db.hpp>
#include <database_module/src/sqlite_db_manager.hpp>

namespace database
{
namespace manager
{

std::shared_ptr<database::IDatabase> SqliteDBManager::Create(const std::string& connectionName)
{
    return std::make_shared<SqliteDB>(connectionName);
}

}  // namespace manager
}  // namespace database
