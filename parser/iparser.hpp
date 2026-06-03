/// @file iparser.hpp
/// @brief Интерфейс парсера
/// @author Artemenko Anton

#ifndef GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5
#define GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5

#include <data_model/src/timeline_data.hpp>
#include <logger/ilogger.hpp>
#include <string>

namespace parser
{
/// @brief Интерфейс парсера данных временного ряда.
class IParser
{
   public:
    /// @brief Конструктор интерфейса парсера.
    /// @param[in] logger Логгер для записи диагностических сообщений.
    explicit IParser(std::shared_ptr<logger::ILogger> logger) : logger_(logger) {}

    /// @brief Виртуальный деструктор.
    virtual ~IParser() = default;

    /// @brief Загрузить временной ряд из источника.
    /// @param[in] source Путь к файлу (для SQLite: "path|tableName").
    /// @return Загруженный временной ряд данных.
    /// @throws ParseException при ошибке открытия или разбора.
    virtual data::TimelineData Load(const std::string& source) = 0;

   protected:
    std::shared_ptr<logger::ILogger> logger_;  ///< Логгер для записи диагностических сообщений.
};
}  // namespace parser
#endif  // GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5
