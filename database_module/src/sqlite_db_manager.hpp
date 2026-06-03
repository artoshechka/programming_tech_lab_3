/// @file sqlite_db_manager.hpp
/// @brief Объявление класса SqliteDBManager
/// @author Artemenko Anton

#ifndef GUID_5551891a_c619_4650_901b_b7244fbe8b95
#define GUID_5551891a_c619_4650_901b_b7244fbe8b95

#include <database_module/idatabase_manager.hpp>

namespace database::manager
{
/// @brief Фабрика SQLite-соединений.
class SqliteDBManager : public IDatabaseManager
{
   public:
    /// @brief Виртуальный деструктор.
    ~SqliteDBManager() override = default;

    /// @brief Создаёт новое SQLite-соединение.
    /// @param[in] connectionName Уникальное имя Qt-соединения.
    /// @return Разделяемый указатель на объект SqliteDB.
    std::shared_ptr<database::IDatabase> Create(const std::string& connectionName) override;
};
}  // namespace database::manager

#endif  // GUID_5551891a_c619_4650_901b_b7244fbe8b95
