#include "pie_chart_builder.hpp"
#include <QtCharts/QPieSeries>
#include <algorithm>

QT_CHARTS_USE_NAMESPACE

namespace chart {

static constexpr int kMaxSlices = 10;

QChart* PieChartBuilder::Build(const data::TimelineData& data) {
    auto pts = data.points_;
    std::sort(pts.begin(), pts.end(), [](const auto& a, const auto& b) {
        return a.value_ > b.value_;
    });

    auto* series = new QPieSeries();
    double other = 0.0;
    for (int i = 0; i < static_cast<int>(pts.size()); ++i) {
        if (i < kMaxSlices)
            series->append(QString::fromStdString(pts[i].time_), pts[i].value_);
        else
            other += pts[i].value_;
    }
    if (other > 0.0)
        series->append("Other", other);

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString::fromStdString(data.name_));
    chart->legend()->setVisible(true);
    return chart;
}

} // namespace chart
