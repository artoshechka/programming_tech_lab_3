/// @file sqlite_db_manager.hpp
/// @brief Объявление класса SqliteDBManager
/// @author Artemenko Anton

#ifndef GUID_5551891a_c619_4650_901b_b7244fbe8b95
#define GUID_5551891a_c619_4650_901b_b7244fbe8b95

#include <database_module/idatabase_manager.hpp>
#include <logger/ilogger.hpp>
#include <memory>

namespace database::manager
{
/// @brief Фабрика SQLite-соединений.
class SqliteDBManager : public IDatabaseManager
{
   public:
    /// @brief Конструктор фабрики.
    /// @param[in] logger Логгер, прокидываемый в каждое создаваемое соединение; допускается nullptr.
    explicit SqliteDBManager(std::shared_ptr<logger::ILogger> logger = nullptr);

    /// @brief Виртуальный деструктор.
    ~SqliteDBManager() override = default;

    /// @brief Создаёт новое SQLite-соединение.
    /// @param[in] connectionName Уникальное имя Qt-соединения.
    /// @return Разделяемый указатель на объект SqliteDB.
    std::shared_ptr<database::IDatabase> Create(const std::string& connectionName) override;

   private:
    std::shared_ptr<logger::ILogger> logger_;  ///< Логгер, прокидываемый в создаваемые соединения.
};
}  // namespace database::manager

#endif  // GUID_5551891a_c619_4650_901b_b7244fbe8b95
