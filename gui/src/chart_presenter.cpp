/// @file chart_presenter.cpp
/// @brief Определение ChartPresenter
/// @author Artemenko Anton

#include <chart/ichart_builder.hpp>
#include <gui/src/chart_presenter.hpp>
#include <logger/logger_macros.hpp>
#include <parser/iparser.hpp>
#include <stdexcept>
#include <system_error>
#include <utility>

namespace gui
{

/// @brief Конструктор презентера.
ChartPresenter::ChartPresenter(BuilderFactory builders, StyleFactory styles,
                               std::shared_ptr<parser::IParserRegistry> registry,
                               std::shared_ptr<logger::ILogger> logger)
    : builders_(std::move(builders)),
      styles_(std::move(styles)),
      registry_(std::move(registry)),
      logger_(std::move(logger))
{
}

/// @brief Возвращает под-источники файла, делегируя парсеру формата.
std::vector<std::string> ChartPresenter::listSubSources(const std::string& path)
{
    // Инвалидация по mtime: пока путь и время модификации совпадают — отдаём из слота, файл не читаем.
    std::error_code ec;
    const auto mtime = std::filesystem::last_write_time(path, ec);
    if (!ec && tablesSlot_ && tablesSlot_->path == path && tablesSlot_->mtime == mtime)
    {
        LogDebug(logger_) << "Sub-sources cache hit: " << path << " (" << tablesSlot_->tables.size() << ")";
        return tablesSlot_->tables;
    }

    auto ext = path.substr(path.rfind('.') + 1);
    for (auto& c : ext) c = static_cast<char>(::tolower(c));
    auto parser = registry_->Get(ext);
    if (!parser) return {};

    LogDebug(logger_) << "Sub-sources cache miss: " << path << ", probing via parser '" << ext << "'";
    auto tables = parser->ListSubSources(path);
    LogInfo(logger_) << "Sub-sources listed: " << path << " -> " << tables.size();

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
    if (ec)
    {
        LogError(logger_) << "Cannot stat source: " << path << ": " << ec.message();
        throw std::invalid_argument("Cannot stat source: " + path + ": " + ec.message());
    }

    // Совпали source и mtime — данные не перечитываем и не парсим, берём из слота.
    if (!dataSlot_ || dataSlot_->source != source || dataSlot_->mtime != mtime)
    {
        LogDebug(logger_) << "Data cache miss: " << source << " (re-parsing)";

        // Другой источник, файл изменился извне или слот пуст — определяем парсер и грузим заново.
        auto ext = path.substr(path.rfind('.') + 1);
        for (auto& c : ext) c = static_cast<char>(::tolower(c));

        auto parser = registry_->Get(ext);
        if (!parser)
        {
            LogError(logger_) << "Unknown format: " << ext << " for source: " << source;
            throw std::invalid_argument("Unknown format: " + ext);
        }

        // Затираем слот свежими данными. parser->Load бросает ParseException — старый слот остаётся нетронутым.
        dataSlot_ = DataSlot{source, mtime, parser->Load(source)};
        LogInfo(logger_) << "Source loaded: " << source << " (" << dataSlot_->data.points_.size() << " points)";
    } else
    {
        LogDebug(logger_) << "Data cache hit: " << source << " (no re-parse)";
    }

    return buildChart(dataSlot_->data, builder, style, aggregate);
}

/// @brief Пересобирает QChart из кэша (без IO).
std::unique_ptr<QChart> ChartPresenter::rebuild(const std::string& builder, const std::string& style, bool aggregate)
{
    if (!dataSlot_)
    {
        LogWarning(logger_) << "Rebuild requested with empty cache — load() must be called first";
        return nullptr;  // ещё ни разу не грузили — пользователь должен сначала load()
    }
    LogInfo(logger_) << "Chart rebuilt from cache: " << dataSlot_->source << " (builder=" << builder
                     << ", style=" << style << ")";
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
