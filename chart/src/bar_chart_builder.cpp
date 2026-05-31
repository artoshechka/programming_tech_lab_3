#include "bar_chart_builder.hpp"
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>

QT_CHARTS_USE_NAMESPACE

namespace chart {

QChart* BarChartBuilder::Build(const data::TimelineData& data) {
    auto* set = new QBarSet(QString::fromStdString(data.name_));
    QStringList categories;
    for (const auto& pt : data.points_) {
        *set << pt.value_;
        categories << QString::fromStdString(pt.time_);
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
