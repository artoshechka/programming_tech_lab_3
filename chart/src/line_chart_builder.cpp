#include "line_chart_builder.hpp"

#include <QDateTime>
#include <QPen>
#include <QString>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QChart>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <algorithm>
#include <logger/logger_macros.hpp>
#include <style/ipalette.hpp>

#include "render_constants.hpp"

QT_CHARTS_USE_NAMESPACE

namespace chart
{

namespace
{

/// @brief Разбирает метку времени в QDateTime, пробуя поддерживаемые форматы.
/// @param[in] raw Сырая строковая метка времени.
/// @return Валидный QDateTime либо невалидный, если ни один формат не подошёл.
QDateTime ParseTime(const std::string& raw)
{
    const QString s = QString::fromStdString(raw);
    QDateTime dt = QDateTime::fromString(s, kTimeFormatFull);
    if (!dt.isValid()) dt = QDateTime::fromString(s, kTimeFormatDay);
    if (!dt.isValid()) dt = QDateTime::fromString(s, Qt::ISODate);
    return dt;
}

}  // namespace

std::unique_ptr<QtCharts::QChart> LineChartBuilder::Build(const data::TimelineData& data)
{
    if (data.points_.empty()) LogWarning(logger_) << "Line build: empty timeline '" << data.name_ << "'";
    LogTrace(logger_) << "Line build: enter, " << data.points_.size() << " points";
    if (palette_ == nullptr) LogWarning(logger_) << "Line build: no palette, falling back to Qt default colors";

    // Линия времени принципиально не агрегирует: суть ряда — все точки value(t) по порядку.
    const bool timeAxis = !data.points_.empty() && ParseTime(data.points_.front().time_).isValid();
    LogDebug(logger_) << "Line options: points=" << data.points_.size() << ", timeAxis=" << timeAxis;

    auto* series = new QLineSeries();
    series->setName(QString::fromStdString(data.name_));

    double minVal = 0.0, maxVal = 0.0;
    bool first = true;
    for (int i = 0; i < static_cast<int>(data.points_.size()); ++i)
    {
        const double v = data.points_[i].value_;
        qreal x = i;
        if (timeAxis)
        {
            const QDateTime dt = ParseTime(data.points_[i].time_);
            if (!dt.isValid()) continue;  // нераспознанные точки на временной оси пропускаем
            x = static_cast<qreal>(dt.toMSecsSinceEpoch());
        }
        series->append(x, v);
        if (first)
        {
            minVal = maxVal = v;
            first = false;
        } else
        {
            minVal = std::min(minVal, v);
            maxVal = std::max(maxVal, v);
        }
    }

    // Покраска линии делегирована палитре: единственная серия — цвет с total=1.
    if (palette_ != nullptr)
    {
        QPen pen = series->pen();
        pen.setColor(palette_->ColorFor(0, 1));
        pen.setWidthF(kSeriesLineWidth);
        series->setPen(pen);
    }

    auto chart = std::make_unique<QChart>();
    chart->addSeries(series);
    chart->setTitle(QString::fromStdString(data.name_));
    chart->legend()->setVisible(false);

    QAbstractAxis* axisX = nullptr;
    if (timeAxis)
    {
        auto* dtAxis = new QDateTimeAxis();
        dtAxis->setFormat(kDayAxisFormat);
        dtAxis->setTickCount(kDateAxisTicks);
        axisX = dtAxis;
    } else
    {
        axisX = new QValueAxis();
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    const double padding = (maxVal - minVal) * kAxisPaddingFraction;
    axisY->setRange(minVal - padding, maxVal + padding);
    axisY->setTickCount(kValueAxisTicks);
    axisY->setGridLineVisible(true);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    LogInfo(logger_) << "Line chart built: '" << data.name_ << "', " << series->count() << " points";
    return chart;
}

}  // namespace chart
