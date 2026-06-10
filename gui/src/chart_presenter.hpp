/// @file chart_presenter.hpp
/// @brief Логика загрузки данных и построения графика
/// @author Artemenko Anton
#ifndef GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891
#define GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891

#include <QtCharts/QChart>
#include <data_model/src/timeline_data.hpp>
#include <filesystem>
#include <gui/src/mainwindow.hpp>
#include <logger/ilogger.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

QT_CHARTS_USE_NAMESPACE

namespace gui
{

/// @brief Загружает данные, кэширует их и строит QChart по текущим builder/style.
class ChartPresenter
{
   public:
    /// @brief Конструктор презентера.
    /// @param[in] builders Фабрика построителей графиков (имя -> создатель).
    /// @param[in] styles Фабрика стилей графиков (имя -> создатель).
    /// @param[in] registry Реестр парсеров для выбора парсера по расширению.
    /// @param[in] logger Логгер для диагностики загрузки и кэширования; допускается nullptr.
    ChartPresenter(BuilderFactory builders, StyleFactory styles, std::shared_ptr<parser::IParserRegistry> registry,
                   std::shared_ptr<logger::ILogger> logger = nullptr);

    /// @brief Возвращает под-источники файла (таблицы и т.п.) для выбора пользователем.
    /// @param[in] path Путь к файлу-источнику.
    /// @return Имена под-источников; пустой вектор, если их нет или формат одиночный.
    /// @details Делегирует парсеру формата по расширению — презентер не зависит от конкретного формата.
    std::vector<std::string> listSubSources(const std::string& path);

    /// @brief Загружает файл, кэширует TimelineData, строит и возвращает QChart.
    /// @details Расширение извлекается из части source до символа '|' (формат "путь" либо "путь|таблица").
    /// @param[in] source Источник данных (путь, опционально с "|таблица" для SQLite).
    /// @param[in] builder Имя построителя графика.
    /// @param[in] style Имя стиля графика.
    /// @param[in] aggregate Включить агрегацию для построителей, которые её поддерживают.
    /// @return Владеющий указатель на построенный график.
    /// @throws parser::ParseException при ошибке парсинга.
    std::unique_ptr<QChart> load(const std::string& source, const std::string& builder, const std::string& style,
                                 bool aggregate);

    /// @brief Пересобирает QChart из кэша (без IO).
    /// @param[in] builder Имя построителя графика.
    /// @param[in] style Имя стиля графика.
    /// @param[in] aggregate Включить агрегацию для построителей, которые её поддерживают.
    /// @return Владеющий указатель на построенный график; nullptr если кэш пуст.
    std::unique_ptr<QChart> rebuild(const std::string& builder, const std::string& style, bool aggregate);

   private:
    /// @brief Единственный слот кэша распарсенных данных последнего источника.
    struct DataSlot
    {
        std::string source;                       ///< Источник, для которого закэшированы данные.
        std::filesystem::file_time_type mtime;    ///< Время последней модификации исходного файла на момент парсинга.
        data::TimelineData data;                  ///< Распарсенные данные.
    };

    /// @brief Единственный слот кэша списка под-источников последнего файла.
    struct TablesSlot
    {
        std::string path;                         ///< Путь к файлу, для которого закэширован список.
        std::filesystem::file_time_type mtime;    ///< Время последней модификации файла на момент чтения.
        std::vector<std::string> tables;          ///< Имена под-источников (таблицы и т.п.).
    };

    /// @brief Строит график из заданных данных по построителю и стилю.
    /// @param[in] data Данные временного ряда.
    /// @param[in] builder Имя построителя графика.
    /// @param[in] style Имя стиля графика.
    /// @param[in] aggregate Включить агрегацию для построителей, которые её поддерживают.
    /// @return Владеющий указатель на построенный график.
    std::unique_ptr<QChart> buildChart(const data::TimelineData& data, const std::string& builder,
                                       const std::string& style, bool aggregate);

    /// @brief Извлекает путь к файлу из source ("путь" либо "путь|таблица").
    static std::string SourcePath(const std::string& source);

    BuilderFactory builders_;                                          ///< Фабрика построителей графиков.
    StyleFactory styles_;                                              ///< Фабрика стилей графиков.
    std::shared_ptr<parser::IParserRegistry> registry_;               ///< Реестр парсеров по расширению.
    std::optional<DataSlot> dataSlot_;                                 ///< Кэш данных последнего источника (один слот).
    std::optional<TablesSlot> tablesSlot_;                            ///< Кэш под-источников последнего файла.
    std::shared_ptr<logger::ILogger> logger_;                         ///< Логгер для диагностики (может быть nullptr).
};

}  // namespace gui

#endif  // GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891
