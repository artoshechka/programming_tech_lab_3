/// @file chart_presenter.cpp
/// @brief Определение ChartPresenter
/// @author Artemenko Anton

#include <gui/src/chart_presenter.hpp>
#include <parser/iparser.hpp>

namespace gui {

/// @brief Конструктор презентера.
ChartPresenter::ChartPresenter(BuilderFactory builders, StyleFactory styles,
                               std::shared_ptr<parser::IParserRegistry> registry)
    : builders_(std::move(builders))
    , styles_(std::move(styles))
    , registry_(std::move(registry))
{}

/// @brief Загружает файл, кэширует TimelineData, строит и возвращает QChart.
QChart* ChartPresenter::load(const std::string& source, const std::string& builder, const std::string& style)
{
    auto ext = source.substr(0, source.find('|'));
    ext = ext.substr(ext.rfind('.') + 1);
    for (auto& c : ext) c = static_cast<char>(::tolower(c));

    auto parser = registry_->Get(ext);
    if (!parser)
        throw std::invalid_argument("Unknown format: " + ext);

    cached_   = parser->Load(source);  // throws ParseException on error
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

} // namespace gui
