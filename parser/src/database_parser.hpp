/// @file database_parser.hpp
/// @brief Объявление парсера базы данных
/// @author Artemenko Anton

#ifndef GUID_d8990b3f_06fb_4bd3_9a73_38fa220978bb
#define GUID_d8990b3f_06fb_4bd3_9a73_38fa220978bb

#include <logger/ilogger.hpp>
#include <memory>
#include <parser/iparser.hpp>

namespace parser
{
/// @brief Парсер для обработки данных базы данных SQLite.
/// @details Имя таблицы трактуется как имя ряда; колонки Time/Value читаются по строковым именам схемы.
class DatabaseParser : public IParser
{
   public:
    /// @brief Конструктор с внедрением логгера.
    /// @param[in] logger Указатель на логгер (копируется, владение разделяется).
    explicit DatabaseParser(std::shared_ptr<logger::ILogger> logger) : IParser(logger)
    {
    }

    ~DatabaseParser() override = default;

    /// @brief Загрузить временной ряд из SQLite-файла.
    /// @param[in] source Путь к файлу базы данных.
    /// @return Загруженный временной ряд.
    data::TimelineData Load(const std::string& source) override;

    /// @brief Сохранить временной ряд в SQLite-файл.
    /// @param[in] data Временной ряд для сохранения.
    /// @param[in] destination Путь к файлу базы данных.
    void Save(const data::TimelineData& data, const std::string& destination) override;
};

}  // namespace parser
#endif  // GUID_d8990b3f_06fb_4bd3_9a73_38fa220978bb
