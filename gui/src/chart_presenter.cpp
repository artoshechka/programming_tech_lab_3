/// @file chart_presenter.cpp
/// @brief Определение ChartPresenter
/// @author Artemenko Anton

#include <atomic>
#include <database_module/idatabase.hpp>
#include <gui/src/chart_presenter.hpp>
#include <parser/iparser.hpp>

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

    static std::atomic<int> probeId{0};
    const std::string connName = "ChartPresenter_probe_" + std::to_string(probeId++);

    auto db = dbManager_->Create(connName);
    if (!db->Open(path)) return {};
    auto tables = db->Tables();
    db->Close();
    return tables;
}

/// @brief Загружает файл, кэширует TimelineData, строит и возвращает QChart.
QChart* ChartPresenter::load(const std::string& source, const std::string& builder, const std::string& style)
{
    auto ext = source.substr(0, source.find('|'));
    ext = ext.substr(ext.rfind('.') + 1);
    for (auto& c : ext) c = static_cast<char>(::tolower(c));

    auto parser = registry_->Get(ext);
    if (!parser) throw std::invalid_argument("Unknown format: " + ext);

    cached_ = parser->Load(source);  // throws ParseException on error
    hasCached_ = true;
    return buildChart(builder, style);
}

/// @brief Пересобирает QChart из кэша (без IO).
QChart* ChartPresenter::rebuild(const std::string& builder, const std::string& style)
{
    if (!hasCached_) return nullptr;
    return buildChart(builder, style);
}

/// @brief Строит график из кэшированных данных по заданным построителю и стилю.
QChart* ChartPresenter::buildChart(const std::string& builder, const std::string& style)
{
    std::unique_ptr<QChart> chart = builders_.at(builder)()->Build(cached_);
    styles_.at(style)()->Apply(chart.get());
    return chart.release();  // владение переходит QChartView (см. MainWindow::setChart)
}

}  // namespace gui
