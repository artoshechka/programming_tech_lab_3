/// @file chart_presenter.hpp
/// @brief Логика загрузки данных и построения графика
/// @author Artemenko Anton
#ifndef GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891
#define GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891

#include <QtCharts/QChart>
#include <data_model/src/timeline_data.hpp>
#include <database_module/idatabase_manager.hpp>
#include <filesystem>
#include <gui/src/lru_cache.hpp>
#include <gui/src/mainwindow.hpp>
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
    /// @brief Запись кэша парсенных данных с отметкой mtime файла-источника.
    struct CacheEntry
    {
        data::TimelineData data;                  ///< Распарсенные данные.
        std::filesystem::file_time_type mtime;    ///< Время последней модификации исходного файла.
    };

    /// @brief Запись кэша списка таблиц SQLite с отметкой mtime файла.
    struct TablesCacheEntry
    {
        std::vector<std::string> tables;          ///< Имена таблиц.
        std::filesystem::file_time_type mtime;    ///< Время последней модификации SQLite-файла.
    };

    /// @brief Строит график из заданных данных по построителю и стилю.
    /// @param[in] data Данные временного ряда.
    /// @param[in] builder Имя построителя графика.
    /// @param[in] style Имя стиля графика.
    /// @param[in] aggregate Включить агрегацию для построителей, которые её поддерживают.
    /// @return Владеющий указатель на построенный график.
    std::unique_ptr<QChart> buildChart(const data::TimelineData& data, const std::string& builder,
                                       const std::string& style, bool aggregate);

    /// @brief Возвращает путь к файлу из source ("path" либо "path|table").
    static std::string SourcePath(const std::string& source);

    /// @brief Размер LRU-кэша распарсенных данных. ~8 файлов — компромисс между памятью и хитами.
    static constexpr std::size_t kDataCacheCapacity = 8;

    /// @brief Размер LRU-кэша списков таблиц SQLite-файлов.
    static constexpr std::size_t kTablesCacheCapacity = 16;

    BuilderFactory builders_;                                                  ///< Фабрика построителей графиков.
    StyleFactory styles_;                                                      ///< Фабрика стилей графиков.
    std::shared_ptr<parser::IParserRegistry> registry_;                        ///< Реестр парсеров по расширению.
    std::shared_ptr<database::manager::IDatabaseManager> dbManager_;           ///< Менеджер БД для инспекции SQLite.
    LruCache<std::string, CacheEntry> dataCache_{kDataCacheCapacity};          ///< LRU-кэш парсенных данных по source.
    LruCache<std::string, TablesCacheEntry> tablesCache_{kTablesCacheCapacity};///< LRU-кэш списков таблиц SQLite по пути.
    std::string lastSource_;                                                   ///< Source последнего успешного load (для rebuild).
};

}  // namespace gui

#endif  // GUID_f1a2b3c4_d5e6_7890_abcd_ef1234567891
