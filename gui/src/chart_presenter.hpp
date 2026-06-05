/// @file chart_presenter.hpp
/// @brief Логика загрузки данных и построения графика
/// @author Artemenko Anton
#ifndef GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891
#define GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891

#include <QtCharts/QChart>
#include <data_model/src/timeline_data.hpp>
#include <database_module/idatabase_manager.hpp>
#include <gui/src/mainwindow.hpp>
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
    /// @param[in] dbManager Менеджер БД для инспекции источников SQLite.
    ChartPresenter(BuilderFactory builders, StyleFactory styles, std::shared_ptr<parser::IParserRegistry> registry,
                   std::shared_ptr<database::manager::IDatabaseManager> dbManager);

    /// @brief Возвращает список таблиц SQLite-файла (для выбора таблицы пользователем).
    /// @param[in] path Путь к файлу базы данных.
    /// @return Имена таблиц; пустой вектор, если файл не открылся или таблиц нет.
    std::vector<std::string> listTables(const std::string& path);

    /// @brief Загружает файл, кэширует TimelineData, строит и возвращает QChart.
    /// @details Расширение извлекается из части source до символа '|' (формат "путь" либо "путь|таблица").
    /// @param[in] source Источник данных (путь, опционально с "|таблица" для SQLite).
    /// @param[in] builder Имя построителя графика.
    /// @param[in] style Имя стиля графика.
    /// @param[in] aggregate Включить агрегацию для построителей, которые её поддерживают.
    /// @return Построенный график.
    /// @throws parser::ParseException при ошибке парсинга.
    QChart* load(const std::string& source, const std::string& builder, const std::string& style, bool aggregate);

    /// @brief Пересобирает QChart из кэша (без IO).
    /// @param[in] builder Имя построителя графика.
    /// @param[in] style Имя стиля графика.
    /// @param[in] aggregate Включить агрегацию для построителей, которые её поддерживают.
    /// @return Построенный график или nullptr если кэш пуст.
    QChart* rebuild(const std::string& builder, const std::string& style, bool aggregate);

   private:
    /// @brief Строит график из кэшированных данных по заданным построителю и стилю.
    /// @param[in] builder Имя построителя графика.
    /// @param[in] style Имя стиля графика.
    /// @param[in] aggregate Включить агрегацию для построителей, которые её поддерживают.
    /// @return Построенный график.
    QChart* buildChart(const std::string& builder, const std::string& style, bool aggregate);

    BuilderFactory builders_;                                         ///< Фабрика построителей графиков.
    StyleFactory styles_;                                             ///< Фабрика стилей графиков.
    std::shared_ptr<parser::IParserRegistry> registry_;               ///< Реестр парсеров по расширению.
    std::shared_ptr<database::manager::IDatabaseManager> dbManager_;  ///< Менеджер БД для инспекции SQLite.
    data::TimelineData cached_;                                       ///< Кэш последних загруженных данных.
    bool hasCached_ = false;                                          ///< Признак наличия валидного кэша.
};

}  // namespace gui

#endif  // GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891
