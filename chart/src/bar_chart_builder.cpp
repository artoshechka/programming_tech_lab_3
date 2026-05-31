#include "bar_chart_builder.hpp"
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>

QT_CHARTS_USE_NAMESPACE

namespace chart {

static constexpr int kMaxBars = 50;

QChart* BarChartBuilder::Build(const data::TimelineData& data) {
    auto* set = new QBarSet(QString::fromStdString(data.name_));
    QStringList categories;

    const int count = std::min(kMaxBars, static_cast<int>(data.points_.size()));
    for (int i = 0; i < count; ++i) {
        *set << data.points_[i].value_;
        categories << QString::fromStdString(data.points_[i].time_);
    }

    auto* series = new QBarSeries();
    series->append(set);

    auto* axis = new QBarCategoryAxis();
    axis->append(categories);

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString::fromStdString(data.name_));
    chart->addAxis(axis, Qt::AlignBottom);
    series->attachAxis(axis);
    chart->legend()->setVisible(false);
    return chart;
}

} // namespace chart
