#include "pie_chart_builder.hpp"

#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <algorithm>
#include <chart/aggregate.hpp>
#include <chart/src/render_constants.hpp>
#include <logger/logger_macros.hpp>
#include <memory>
#include <style/ipalette.hpp>

QT_CHARTS_USE_NAMESPACE

namespace chart
{

std::unique_ptr<QtCharts::QChart> PieChartBuilder::Build(const data::TimelineData& raw)
{
    if (raw.points_.empty()) LogWarning(logger_) << "Pie build: empty timeline '" << raw.name_ << "'";
    LogTrace(logger_) << "Pie build: enter, " << raw.points_.size() << " points";

    const bool willAggregate = aggregate_ && raw.points_.size() > kAggregateThreshold;
    LogDebug(logger_) << "Pie options: aggregate=" << aggregate_ << ", points=" << raw.points_.size()
                      << ", threshold=" << kAggregateThreshold << " -> aggregating=" << willAggregate;
    if (palette_ == nullptr) LogWarning(logger_) << "Pie build: no palette, falling back to Qt default colors";

    // агрегация и сжатие хвоста в "Other" применяются только при включённом флаге
    const data::TimelineData agg = willAggregate ? Aggregate(raw) : raw;

    auto pts = agg.points_;
    std::sort(pts.begin(), pts.end(), [](const auto& a, const auto& b) { return a.value_ > b.value_; });

    auto series = std::make_unique<QPieSeries>();
    if (aggregate_)
    {
        double other = 0.0;
        for (int i = 0; i < static_cast<int>(pts.size()); ++i)
        {
            if (i < kMaxSlices)
                series->append(QString::fromStdString(pts[i].time_), pts[i].value_);
            else
                other += pts[i].value_;
        }
        if (other > 0.0) series->append("Other", other);
    } else
    {
        for (const auto& pt : pts) series->append(QString::fromStdString(pt.time_), pt.value_);
    }

    series->setLabelsVisible(true);
    series->setLabelsPosition(QPieSlice::LabelOutside);

    // Покраска срезов делегирована палитре стиля; без палитры — оставляем Qt-дефолт.
    if (palette_ != nullptr)
    {
        const auto slices = series->slices();
        const int n = slices.size();
        for (int i = 0; i < n; ++i)
        {
            slices[i]->setColor(palette_->ColorFor(i, n));
            slices[i]->setBorderColor(Qt::white);
            slices[i]->setLabelColor(Qt::black);
        }
    }

    auto chart = std::make_unique<QChart>();
    auto* seriesPtr = series.get();
    chart->addSeries(series.release());
    chart->setTitle(QString::fromStdString(agg.name_));
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setVisible(true);
    LogInfo(logger_) << "Pie chart built: '" << agg.name_ << "', " << seriesPtr->count() << " slices";
    return chart;
}

}  // namespace chart
