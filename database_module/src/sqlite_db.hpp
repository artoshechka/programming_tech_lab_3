/// @file sqlite_db.hpp
/// @brief Реализация IDatabase для SQLite через Qt SQL.
/// @author Artemenko Anton

#ifndef GUID_652d92c5_34e4_4c88_9e56_d4bf9ed33d6d
#define GUID_652d92c5_34e4_4c88_9e56_d4bf9ed33d6d

#include <QSqlDatabase>
#include <database_module/idatabase.hpp>
#include <logger/ilogger.hpp>
#include <memory>
#include <string>

namespace database
{
/// @brief Реализация IDatabase для SQLite через Qt SQL.
class SqliteDB : public IDatabase
{
   public:
    /// @brief Создаёт соединение SQLite с заданным именем.
    /// @param[in] connectionName Уникальное имя Qt-соединения.
    /// @param[in] logger Логгер для диагностики открытия и запросов; допускается nullptr.
    /// @details Имя connectionName используется при регистрации соединения в Qt SQL
    ///          и должно быть уникальным в пределах процесса.
    explicit SqliteDB(const std::string& connectionName, const std::shared_ptr<logger::ILogger>& logger = nullptr);

    /// @brief Виртуальный деструктор.
    /// @details Закрывает соединение и удаляет его из реестра Qt SQL.
    ~SqliteDB() override;

    /// @brief Открывает базу данных по указанному пути.
    /// @param[in] path Путь к файлу базы данных SQLite.
    /// @return true при успешном открытии, иначе false.
    bool Open(const std::string& path) override;

    /// @brief Закрывает текущее соединение с базой данных.
    void Close() override;

    /// @brief Возвращает список таблиц в открытой базе данных.
    /// @return Вектор имён таблиц.
    std::vector<std::string> Tables() const override;

    /// @brief Выполняет SQL-запрос и передаёт каждую ячейку результата через визитор.
    /// @param[in] sql Текст SQL-запроса.
    /// @param[in] rowFn Визитор, вызываемый для каждой ячейки строки результата.
    /// @throws std::runtime_error при сбое query.exec() с текстом QSqlError::text().
    void Query(const std::string& sql,
               std::function<void(const std::string& col, const std::string& val)> rowFn) override;

   private:
    QSqlDatabase db_;                          ///< Qt-объект соединения с базой данных SQLite.
    std::shared_ptr<logger::ILogger> logger_;  ///< Логгер для диагностики (может быть nullptr).
};
}  // namespace database
#endif  // GUID_652d92c5_34e4_4c88_9e56_d4bf9ed33d6d
