/// @file json_parser.hpp
/// @brief Объявление парсера JSON
/// @author Artemenko Anton

#ifndef GUID_cf64fc0e_c3bd_4922_8d74_fae170ae2ea0
#define GUID_cf64fc0e_c3bd_4922_8d74_fae170ae2ea0

#include <parser/parser_base.hpp>

namespace parser
{
/// @brief Парсер JSON.
/// @details Формат: { "name": <строка>, "points": [ { "Time": <строка>, "Value": <число> } ] }.
class JsonParser : public ParserBase
{
   public:
    /// @brief Конструктор парсера JSON.
    /// @param[in] logger Логгер для записи диагностических сообщений.
    explicit JsonParser(std::shared_ptr<logger::ILogger> logger) : ParserBase(std::move(logger))
    {
    }

    /// @brief Виртуальный деструктор.
    ~JsonParser() override = default;

    /// @brief Загрузить временной ряд из JSON-файла.
    /// @param[in] source Путь к JSON-файлу.
    /// @return Загруженный временной ряд данных.
    /// @throws ParseException при ошибке открытия или разбора.
    data::TimelineData Load(const std::string& source) override;
};
}  // namespace parser
#endif  // GUID_cf64fc0e_c3bd_4922_8d74_fae170ae2ea0
