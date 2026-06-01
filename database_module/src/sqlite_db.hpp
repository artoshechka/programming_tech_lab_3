/// @file sqlite_db.hpp
/// @brief Объявление класса SqliteDB
/// @author Artemenko Anton

#ifndef GUID_652d92c5_34e4_4c88_9e56_d4bf9ed33d6d
#define GUID_652d92c5_34e4_4c88_9e56_d4bf9ed33d6d

#include <database_module/idatabase.hpp>
#include <QSqlDatabase>
#include <string>

namespace database
{
/// @brief Реализация IDatabase для SQLite через Qt SQL.
class SqliteDB : public IDatabase
{
   public:
    /// @brief Создаёт соединение с заданным именем.
    /// @param connectionName Уникальное имя Qt-соединения.
    explicit SqliteDB(const std::string& connectionName);
    ~SqliteDB() override;

    bool Open(const std::string& path) override;
    void Close() override;
    std::vector<std::string> Tables() const override;
    void Query(const std::string& sql,
               std::function<void(const std::string& col, const std::string& val)> rowFn) override;

   private:
    QSqlDatabase db_;
};
}  // namespace database
#endif  // GUID_652d92c5_34e4_4c88_9e56_d4bf9ed33d6d
