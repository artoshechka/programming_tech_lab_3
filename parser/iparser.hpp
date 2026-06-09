/// @file iparser.hpp
/// @brief Интерфейс парсера
/// @author Artemenko Anton

#ifndef GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5
#define GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5

#include <data_model/src/timeline_data.hpp>
#include <string>
#include <vector>

namespace parser
{
/// @brief Чистый интерфейс парсера данных временного ряда.
class IParser
{
   public:
    /// @brief Виртуальный деструктор.
    virtual ~IParser() = default;

    /// @brief Загрузить временной ряд из источника.
    /// @param[in] source Путь к файлу (для SQLite: "path|tableName").
    /// @return Загруженный временной ряд данных.
    /// @throws ParseException при ошибке открытия или разбора.
    virtual data::TimelineData Load(const std::string& source) = 0;

    /// @brief Перечислить под-источники внутри файла (таблицы БД, листы и т.п.).
    /// @param[in] path Путь к файлу-источнику (без хвоста "|...").
    /// @return Имена под-источников; пустой вектор — формат одиночный (по умолчанию).
    /// @details Базовая реализация — no-op: парсеры без под-источников (JSON и т.п.) её не
    ///          переопределяют. Позволяет GUI выбирать под-источник, не зная о формате.
    virtual std::vector<std::string> ListSubSources(const std::string& path) const
    {
        (void)path;
        return {};
    }
};
}  // namespace parser
#endif  // GUID_9b1c8a7e_5c3d_4c8a_9a12_6b3f8d0e71c5
