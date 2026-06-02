/// @file chart_presenter.cpp
/// @brief Определение ChartPresenter
/// @author Artemenko Anton

#include <gui/src/chart_presenter.hpp>
#include <parser/iparser.hpp>

namespace gui {

ChartPresenter::ChartPresenter(BuilderFactory builders, StyleFactory styles,
                               std::shared_ptr<parser::IParserRegistry> registry)
    : builders_(std::move(builders))
    , styles_(std::move(styles))
    , registry_(std::move(registry))
{}

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

QChart* ChartPresenter::rebuild(const std::string& builder, const std::string& style)
{
    if (!hasCached_) return nullptr;
    return buildChart(builder, style);
}

QChart* ChartPresenter::buildChart(const std::string& builder, const std::string& style)
{
    QChart* chart = builders_.at(builder)()->Build(cached_);
    styles_.at(style)()->Apply(chart);
    return chart;
}

} // namespace gui
