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

    // Инвалидация по mtime: пока путь и время модификации совпадают — отдаём из слота, БД не открываем.
    std::error_code ec;
    const auto mtime = std::filesystem::last_write_time(path, ec);
    if (!ec && tablesSlot_ && tablesSlot_->path == path && tablesSlot_->mtime == mtime)
    {
        return tablesSlot_->tables;
    }

    static std::atomic<int> probeId{0};
    const std::string connName = "ChartPresenter_probe_" + std::to_string(probeId++);

    auto db = dbManager_->Create(connName);
    if (!db->Open(path)) return {};
    auto tables = db->Tables();

    // Перечитали — затираем слот свежим значением (если удалось получить mtime).
    if (!ec) tablesSlot_ = TablesSlot{path, mtime, tables};
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

    // Совпали source и mtime — данные не перечитываем и не парсим, берём из слота.
    if (!dataSlot_ || dataSlot_->source != source || dataSlot_->mtime != mtime)
    {
        // Другой источник, файл изменился извне или слот пуст — определяем парсер и грузим заново.
        auto ext = path.substr(path.rfind('.') + 1);
        for (auto& c : ext) c = static_cast<char>(::tolower(c));

        auto parser = registry_->Get(ext);
        if (!parser) throw std::invalid_argument("Unknown format: " + ext);

        // Затираем слот свежими данными. parser->Load бросает ParseException — старый слот остаётся нетронутым.
        dataSlot_ = DataSlot{source, mtime, parser->Load(source)};
    }

    return buildChart(dataSlot_->data, builder, style, aggregate);
}

/// @brief Пересобирает QChart из кэша (без IO).
std::unique_ptr<QChart> ChartPresenter::rebuild(const std::string& builder, const std::string& style, bool aggregate)
{
    if (!dataSlot_) return nullptr;  // ещё ни разу не грузили — пользователь должен сначала load()
    return buildChart(dataSlot_->data, builder, style, aggregate);
}

/// @brief Строит график из заданных данных по заданным построителю и стилю.
std::unique_ptr<QChart> ChartPresenter::buildChart(const data::TimelineData& data, const std::string& builder,
                                                   const std::string& style, bool aggregate)
{
    auto chartStyle = styles_.at(style)();
    auto chartBuilder = builders_.at(builder)();
    chartBuilder->Configure(chart::BuilderOptions{aggregate, chartStyle.get()});
    std::unique_ptr<QChart> chart = chartBuilder->Build(data);
    chartStyle->Apply(chart.get());  // общие настройки графика (тема/фон)
    return chart;  // владение передаётся вызывающему (MainWindow::setChart) через std::unique_ptr.
}

/// @brief Возвращает путь из source (вырезает "|table" хвост для SQLite).
std::string ChartPresenter::SourcePath(const std::string& source)
{
    const auto pos = source.find('|');
    return pos == std::string::npos ? source : source.substr(0, pos);
}

}  // namespace gui
