#include "pie_chart_builder.hpp"
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

QT_CHARTS_USE_NAMESPACE

namespace chart {

QChart* PieChartBuilder::Build(const data::TimelineData& data) {
    auto* series = new QPieSeries();
    for (const auto& pt : data.points_) {
        series->append(QString::fromStdString(pt.time_), pt.value_);
    }

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString::fromStdString(data.name_));
    chart->legend()->setVisible(true);
    return chart;
}

} // namespace chart
