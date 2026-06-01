#include "bar_chart_builder.hpp"
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

namespace chart {

static constexpr int kMaxBars = 24;

QChart* BarChartBuilder::Build(const data::TimelineData& data) {
    const int count = std::min(kMaxBars, static_cast<int>(data.points_.size()));

    auto* set = new QBarSet(QString::fromStdString(data.name_));
    QStringList categories;
    double minVal = 0.0, maxVal = 0.0;
    for (int i = 0; i < count; ++i) {
        double v = data.points_[i].value_;
        *set << v;
        categories << QString::fromStdString(data.points_[i].time_);
        if (i == 0) { minVal = maxVal = v; }
        else { minVal = std::min(minVal, v); maxVal = std::max(maxVal, v); }
    }

    auto* series = new QBarSeries();
    series->append(set);
    series->setLabelsVisible(true);
    series->setLabelsFormat("@value");
    series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);

    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);

    auto* axisY = new QValueAxis();
    double padding = (maxVal - minVal) * 0.15;
    axisY->setRange(std::min(0.0, minVal - padding), maxVal + padding);
    axisY->setTickCount(6);
    axisY->setGridLineVisible(true);

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString::fromStdString(data.name_));
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chart->legend()->setVisible(false);
    return chart;
}

} // namespace chart
