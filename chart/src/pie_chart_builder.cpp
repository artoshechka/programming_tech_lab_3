#include "pie_chart_builder.hpp"

#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <algorithm>
#include <chart/aggregate.hpp>

QT_CHARTS_USE_NAMESPACE

namespace chart
{

static constexpr int kMaxSlices = 10;

std::unique_ptr<QtCharts::QChart> PieChartBuilder::Build(const data::TimelineData& raw)
{
    // агрегация и сжатие хвоста в "Other" применяются только при включённом флаге
    const data::TimelineData agg =
        (aggregate_ && raw.points_.size() > kAggregateThreshold) ? Aggregate(raw) : raw;

    auto pts = agg.points_;
    std::sort(pts.begin(), pts.end(), [](const auto& a, const auto& b) { return a.value_ > b.value_; });

    auto* series = new QPieSeries();
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

    auto chart = std::make_unique<QChart>();
    chart->addSeries(series);
    chart->setTitle(QString::fromStdString(agg.name_));
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setVisible(true);
    return chart;
}

}  // namespace chart
