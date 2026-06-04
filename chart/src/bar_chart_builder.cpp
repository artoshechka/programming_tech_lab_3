#include "bar_chart_builder.hpp"

#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <chart/aggregate.hpp>

QT_CHARTS_USE_NAMESPACE

namespace chart
{

std::unique_ptr<QChart> BarChartBuilder::Build(const data::TimelineData& raw)
{
    // агрегируем по месяцу если точек много, иначе берём как есть
    const data::TimelineData agg_ = (raw.points_.size() > kAggregateThreshold) ? Aggregate(raw) : raw;
    const data::TimelineData& data = agg_;

    auto* set = new QBarSet(QString::fromStdString(data.name_));
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

    auto* series = new QBarSeries();
    series->append(set);

    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);

    auto* axisY = new QValueAxis();
    double padding = (maxVal - minVal) * 0.15;
    axisY->setRange(std::min(0.0, minVal - padding), maxVal + padding);
    axisY->setTickCount(6);
    axisY->setGridLineVisible(true);

    auto chart = std::make_unique<QChart>();
    chart->addSeries(series);
    chart->setTitle(QString::fromStdString(data.name_));
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chart->legend()->setVisible(false);
    return chart;
}

}  // namespace chart
