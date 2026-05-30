/// @file json_parser.hpp
/// @brief Объявление парсера JSON
/// @author Artemenko Anton

#ifndef GUID_cf64fc0e_c3bd_4922_8d74_fae170ae2ea0
#define GUID_cf64fc0e_c3bd_4922_8d74_fae170ae2ea0

#include <logger/ilogger.hpp>
#include <memory>
#include <parser/iparser.hpp>

namespace parser
{
/// @brief Парсер для обработки JSON данных.
/// @details Формат JSON: { "name": <строка>, "points": [ { "Time": <строка>, "Value": <число> } ] }.
class JsonParser : public IParser
{
   public:
    /// @brief Конструктор с внедрением логгера.
    /// @param[in] logger Указатель на логгер (копируется, владение разделяется).
    explicit JsonParser(std::shared_ptr<logger::ILogger> logger) : IParser(logger)
    {
    }

    ~JsonParser() override = default;

    /// @brief Загрузить временной ряд из JSON-файла.
    /// @param[in] source Путь к JSON-файлу.
    /// @return Загруженный временной ряд.
    data::TimelineData Load(const std::string& source) override;

    /// @brief Сохранить временной ряд в JSON-файл.
    /// @param[in] data Временной ряд для сохранения.
    /// @param[in] destination Путь к JSON-файлу.
    void Save(const data::TimelineData& data, const std::string& destination) override;
};

}  // namespace parser
#endif  // GUID_cf64fc0e_c3bd_4922_8d74_fae170ae2ea0
