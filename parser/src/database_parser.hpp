/// @file database_parser.hpp
/// @brief Объявление парсера базы данных
/// @author Artemenko Anton

#ifndef GUID_d8990b3f_06fb_4bd3_9a73_38fa220978bb
#define GUID_d8990b3f_06fb_4bd3_9a73_38fa220978bb

#include <database_module/idatabase_manager.hpp>
#include <parser/iparser.hpp>

namespace parser
{
/// @brief Парсер SQLite через IDatabaseManager.
/// @details source передаётся как "path|tableName". Если tableName отсутствует — берётся первая таблица.
class DatabaseParser : public IParser
{
   public:
    /// @brief Конструктор парсера базы данных.
    /// @param[in] logger Логгер для записи диагностических сообщений.
    /// @param[in] manager Фабрика соединений с БД.
    DatabaseParser(std::shared_ptr<logger::ILogger> logger,
                   std::shared_ptr<database::manager::IDatabaseManager> manager)
        : IParser(logger), manager_(std::move(manager))
    {
    }

    /// @brief Виртуальный деструктор.
    ~DatabaseParser() override = default;

    /// @brief Загрузить временной ряд из таблицы БД.
    /// @param[in] source Источник в формате "path|tableName"; если tableName опущен — берётся первая таблица.
    /// @return Загруженный временной ряд данных.
    /// @throws ParseException при ошибке открытия или разбора.
    data::TimelineData Load(const std::string& source) override;

   private:
    std::shared_ptr<database::manager::IDatabaseManager> manager_;  ///< Фабрика соединений с БД.

    static int nextConnId_;  ///< Счётчик для генерации уникальных имён соединений.
};
}  // namespace parser
#endif  // GUID_d8990b3f_06fb_4bd3_9a73_38fa220978bb
