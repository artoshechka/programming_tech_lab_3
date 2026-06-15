#include "bar_chart_builder.hpp"

#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <chart/aggregate.hpp>
#include <memory>
#include <logger/logger_macros.hpp>
#include <style/ipalette.hpp>

QT_CHARTS_USE_NAMESPACE

namespace chart
{

std::unique_ptr<QtCharts::QChart> BarChartBuilder::Build(const data::TimelineData& raw)
{
    if (raw.points_.empty()) LogWarning(logger_) << "Bar build: empty timeline '" << raw.name_ << "'";
    LogTrace(logger_) << "Bar build: enter, " << raw.points_.size() << " points";

    const bool willAggregate = raw.points_.size() > kAggregateThreshold;
    LogDebug(logger_) << "Bar options: points=" << raw.points_.size() << ", threshold=" << kAggregateThreshold
                      << " -> aggregating=" << willAggregate;
    if (palette_ == nullptr) LogWarning(logger_) << "Bar build: no palette, falling back to Qt default colors";

    // агрегируем по месяцу если точек много, иначе берём как есть
    const data::TimelineData agg_ = willAggregate ? Aggregate(raw) : raw;
    const data::TimelineData& data = agg_;

    auto set = std::make_unique<QBarSet>(QString::fromStdString(data.name_));
    QStringList categories;
    double minVal = 0.0, maxVal = 0.0;

    for (int i = 0; i < static_cast<int>(data.points_.size()); ++i)
    {
        double v = data.points_[i].value_;
        *set << v;
        categories << QString::fromStdString(data.points_[i].time_);
        if (i == 0)
        {
            minVal = maxVal = v;
        } else
        {
            minVal = std::min(minVal, v);
            maxVal = std::max(maxVal, v);
        }
    }

    // Bar-чарт использует один QBarSet — палитра выдаёт единственный цвет с total=1.
    if (palette_ != nullptr) set->setColor(palette_->ColorFor(0, 1));

    auto series = std::make_unique<QBarSeries>();
    series->append(set.release());

    auto axisX = std::make_unique<QBarCategoryAxis>();
    axisX->append(categories);

    auto axisY = std::make_unique<QValueAxis>();
    double padding = (maxVal - minVal) * 0.15;
    axisY->setRange(std::min(0.0, minVal - padding), maxVal + padding);
    axisY->setTickCount(6);
    axisY->setGridLineVisible(true);

    auto chart = std::make_unique<QChart>();
    auto* seriesPtr = series.get();
    chart->addSeries(series.release());
    chart->setTitle(QString::fromStdString(data.name_));
    auto* axisXPtr = axisX.get();
    chart->addAxis(axisX.release(), Qt::AlignBottom);
    auto* axisYPtr = axisY.get();
    chart->addAxis(axisY.release(), Qt::AlignLeft);
    seriesPtr->attachAxis(axisXPtr);
    seriesPtr->attachAxis(axisYPtr);
    chart->legend()->setVisible(false);
    LogInfo(logger_) << "Bar chart built: '" << data.name_ << "', " << data.points_.size() << " bars";
    return chart;
}

}  // namespace chart
