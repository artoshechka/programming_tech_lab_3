#include "area_chart_builder.hpp"

#include <QBrush>
#include <QColor>
#include <QDateTime>
#include <QPen>
#include <QString>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QChart>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <algorithm>
#include <memory>
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

std::unique_ptr<QtCharts::QChart> AreaChartBuilder::Build(const data::TimelineData& data)
{
    if (data.points_.empty()) LogWarning(logger_) << "Area build: empty timeline '" << data.name_ << "'";
    LogTrace(logger_) << "Area build: enter, " << data.points_.size() << " points";
    if (palette_ == nullptr) LogWarning(logger_) << "Area build: no palette, falling back to Qt default colors";

    // Как и линия времени, область строится по всем точкам без агрегации.
    const bool timeAxis = !data.points_.empty() && ParseTime(data.points_.front().time_).isValid();
    LogDebug(logger_) << "Area options: points=" << data.points_.size() << ", timeAxis=" << timeAxis;

    auto upper = std::make_unique<QLineSeries>();  // верхняя граница области = кривая value(t)

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
        upper->append(x, v);
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

    auto* upperPtr = upper.get();
    auto series = std::make_unique<QAreaSeries>(upper.release());
    series->setName(QString::fromStdString(data.name_));

    // Покраска области делегирована палитре: контур — насыщенный цвет, заливка — полупрозрачная.
    if (palette_ != nullptr)
    {
        const QColor color = palette_->ColorFor(0, 1);
        QPen pen = series->pen();
        pen.setColor(color);
        pen.setWidthF(kSeriesLineWidth);
        series->setPen(pen);
        QColor fill = color;
        fill.setAlpha(kAreaFillAlpha);
        series->setBrush(QBrush(fill));
    }

    auto chart = std::make_unique<QChart>();
    auto* seriesPtr = series.get();
    chart->addSeries(series.release());
    chart->setTitle(QString::fromStdString(data.name_));
    chart->legend()->setVisible(false);

    std::unique_ptr<QAbstractAxis> axisX;
    if (timeAxis)
    {
        auto dtAxis = std::make_unique<QDateTimeAxis>();
        dtAxis->setFormat(kDayAxisFormat);
        dtAxis->setTickCount(kDateAxisTicks);
        axisX = std::move(dtAxis);
    } else
    {
        axisX = std::make_unique<QValueAxis>();
    }
    auto* axisXPtr = axisX.get();
    chart->addAxis(axisX.release(), Qt::AlignBottom);
    seriesPtr->attachAxis(axisXPtr);

    auto axisY = std::make_unique<QValueAxis>();
    const double padding = (maxVal - minVal) * kAxisPaddingFraction;
    axisY->setRange(minVal - padding, maxVal + padding);
    axisY->setTickCount(kValueAxisTicks);
    axisY->setGridLineVisible(true);
    auto* axisYPtr = axisY.get();
    chart->addAxis(axisY.release(), Qt::AlignLeft);
    seriesPtr->attachAxis(axisYPtr);

    LogInfo(logger_) << "Area chart built: '" << data.name_ << "', " << upperPtr->count() << " points";
    return chart;
}

}  // namespace chart
