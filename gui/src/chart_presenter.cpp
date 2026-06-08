/// @file chart_presenter.cpp
/// @brief Определение ChartPresenter
/// @author Artemenko Anton

#include <atomic>
#include <chart/ichart_builder.hpp>
#include <database_module/idatabase.hpp>
#include <gui/src/chart_presenter.hpp>
#include <parser/iparser.hpp>
#include <stdexcept>
#include <system_error>

namespace gui
{

/// @brief Конструктор презентера.
ChartPresenter::ChartPresenter(BuilderFactory builders, StyleFactory styles,
                               std::shared_ptr<parser::IParserRegistry> registry,
                               std::shared_ptr<database::manager::IDatabaseManager> dbManager)
    : builders_(std::move(builders)),
      styles_(std::move(styles)),
      registry_(std::move(registry)),
      dbManager_(std::move(dbManager))
{
}

/// @brief Возвращает список таблиц SQLite-файла.
std::vector<std::string> ChartPresenter::listTables(const std::string& path)
{
    if (!dbManager_) return {};

    // Инвалидация по mtime: если файл изменился извне, кэш считаем устаревшим.
    std::error_code ec;
    const auto mtime = std::filesystem::last_write_time(path, ec);
    if (!ec)
    {
        if (const TablesCacheEntry* hit = tablesCache_.Find(path); hit != nullptr && hit->mtime == mtime)
        {
            return hit->tables;
        }
    }

    static std::atomic<int> probeId{0};
    const std::string connName = "ChartPresenter_probe_" + std::to_string(probeId++);

    auto db = dbManager_->Create(connName);
    if (!db->Open(path)) return {};
    auto tables = db->Tables();

    if (!ec) tablesCache_.Put(path, TablesCacheEntry{tables, mtime});
    return tables;
}

/// @brief Загружает файл, кэширует TimelineData, строит и возвращает QChart.
std::unique_ptr<QChart> ChartPresenter::load(const std::string& source, const std::string& builder,
                                             const std::string& style, bool aggregate)
{
    const std::string path = SourcePath(source);

    // Проверяем mtime файла-источника для инвалидации кэша при внешних изменениях.
    std::error_code ec;
    const auto mtime = std::filesystem::last_write_time(path, ec);
    if (ec) throw std::invalid_argument("Cannot stat source: " + path + ": " + ec.message());

    const CacheEntry* entry = dataCache_.Find(source);
    if (entry == nullptr || entry->mtime != mtime)
    {
        // Промах или устарело — определяем парсер по расширению и грузим заново.
        auto ext = path.substr(path.rfind('.') + 1);
        for (auto& c : ext) c = static_cast<char>(::tolower(c));

        auto parser = registry_->Get(ext);
        if (!parser) throw std::invalid_argument("Unknown format: " + ext);

        dataCache_.Put(source, CacheEntry{parser->Load(source), mtime});  // throws ParseException
        entry = dataCache_.Find(source);
    }

    lastSource_ = source;
    return buildChart(entry->data, builder, style, aggregate);
}

/// @brief Пересобирает QChart из кэша (без IO).
std::unique_ptr<QChart> ChartPresenter::rebuild(const std::string& builder, const std::string& style, bool aggregate)
{
    if (lastSource_.empty()) return nullptr;
    const CacheEntry* entry = dataCache_.Find(lastSource_);
    if (entry == nullptr) return nullptr;  // вытеснили из LRU — пользователь должен load() заново
    return buildChart(entry->data, builder, style, aggregate);
}

/// @brief Строит график из заданных данных по заданным построителю и стилю.
std::unique_ptr<QChart> ChartPresenter::buildChart(const data::TimelineData& data, const std::string& builder,
                                                   const std::string& style, bool aggregate)
{
    auto chartBuilder = builders_.at(builder)();
    chartBuilder->Configure(chart::BuilderOptions{aggregate});
    std::unique_ptr<QChart> chart = chartBuilder->Build(data);
    styles_.at(style)()->Apply(chart.get());
    return chart;  // владение передаётся вызывающему (MainWindow::setChart) через std::unique_ptr.
}

/// @brief Возвращает путь из source (вырезает "|table" хвост для SQLite).
std::string ChartPresenter::SourcePath(const std::string& source)
{
    const auto pos = source.find('|');
    return pos == std::string::npos ? source : source.substr(0, pos);
}

}  // namespace gui
