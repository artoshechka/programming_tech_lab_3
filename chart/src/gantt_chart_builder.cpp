#include "gantt_chart_builder.hpp"

#include <QDate>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QHorizontalStackedBarSeries>
#include <QtCharts/QValueAxis>
#include <algorithm>
#include <chart/aggregate.hpp>
#include <logger/logger_macros.hpp>
#include <style/ipalette.hpp>
#include <vector>

#include "render_constants.hpp"

QT_CHARTS_USE_NAMESPACE

namespace chart
{

namespace
{

/// @brief Длина полосы периода в днях (визуальная длительность одного месяца на оси).
constexpr qint64 kBarDays = 26;
/// @brief Миллисекунд в сутках.
constexpr qint64 kMsecsPerDay = 24LL * 60 * 60 * 1000;
/// @brief Длина полосы на порядковой оси (доля ширины категории), когда даты не распознаны.
constexpr double kOrdinalBarLen = 0.8;
/// @brief Ключ периода в ISO-формате «ГГГГ-ММ» (фолбэк к помесячному формату БД).
constexpr const char* kIsoMonthKey = "yyyy-MM";

/// @brief Разбирает ключ периода агрегации в дату начала месяца.
/// @param[in] key Ключ периода ("MM.YYYY" из БД-формата либо "YYYY-MM" из ISO).
/// @return Валидная QDate (1-е число месяца) либо невалидная, если формат не распознан.
QDate ParsePeriod(const std::string& key)
{
    const QString s = QString::fromStdString(key);
    QDate d = QDate::fromString(s, kMonthAxisFormat);
    if (!d.isValid()) d = QDate::fromString(s, kIsoMonthKey);
    return d;
}

}  // namespace

std::unique_ptr<QtCharts::QChart> GanttChartBuilder::Build(const data::TimelineData& raw)
{
    if (raw.points_.empty()) LogWarning(logger_) << "Gantt build: empty timeline '" << raw.name_ << "'";
    LogTrace(logger_) << "Gantt build: enter, " << raw.points_.size() << " points";
    if (palette_ == nullptr) LogWarning(logger_) << "Gantt build: no palette, falling back to Qt default colors";

    // Таймлайн всегда строится по периодам — агрегируем точки по месяцу.
    const data::TimelineData data = Aggregate(raw);
    const int n = static_cast<int>(data.points_.size());
    LogDebug(logger_) << "Gantt options: periods=" << n;

    // Распознаём, лежат ли периоды на реальной оси времени; иначе используем порядковую ось.
    const bool timeAxis = n > 0 && ParsePeriod(data.points_.front().time_).isValid();
    const qint64 barMsecs = kBarDays * kMsecsPerDay;

    auto* offsetSet = new QBarSet("");  // прозрачный сегмент-смещение задаёт позицию полосы
    QStringList categories;

    // Левые границы (смещения) и категории-подписи для каждого периода.
    std::vector<double> offsets(n, 0.0);
    for (int i = 0; i < n; ++i)
    {
        categories << QString::fromStdString(data.points_[i].time_);
        if (timeAxis)
        {
            const QDate d = ParsePeriod(data.points_[i].time_);
            offsets[i] = d.isValid() ? static_cast<double>(QDateTime(d, QTime(0, 0)).toMSecsSinceEpoch()) : 0.0;
        } else
        {
            offsets[i] = static_cast<double>(i);
        }
        *offsetSet << offsets[i];
    }
    offsetSet->setColor(Qt::transparent);
    offsetSet->setBorderColor(Qt::transparent);

    auto* series = new QHorizontalStackedBarSeries();
    series->append(offsetSet);

    const double barLen = timeAxis ? static_cast<double>(barMsecs) : kOrdinalBarLen;

    // По одному цветному набору на период: ненулевая длина только в своей категории,
    // что позволяет красить каждую полосу отдельным цветом палитры.
    double maxRight = 0.0;
    for (int i = 0; i < n; ++i)
    {
        auto* barSet = new QBarSet(QString::fromStdString(data.points_[i].time_));
        for (int j = 0; j < n; ++j) *barSet << (j == i ? barLen : 0.0);
        if (palette_ != nullptr)
        {
            barSet->setColor(palette_->ColorFor(i, n));
            barSet->setBorderColor(Qt::transparent);
        }
        series->append(barSet);
        maxRight = std::max(maxRight, offsets[i] + barLen);
    }
    series->setLabelsVisible(false);

    auto chart = std::make_unique<QChart>();
    chart->addSeries(series);
    chart->setTitle(QString::fromStdString(data.name_));
    chart->legend()->setVisible(false);

    auto* axisY = new QBarCategoryAxis();
    axisY->append(categories);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    if (timeAxis)
    {
        auto* axisX = new QDateTimeAxis();
        axisX->setFormat(kMonthAxisFormat);
        axisX->setTickCount(kDateAxisTicks);
        axisX->setRange(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(offsets.front())),
                        QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(maxRight)));
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
    } else
    {
        auto* axisX = new QValueAxis();
        axisX->setRange(0.0, maxRight);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
    }

    LogInfo(logger_) << "Gantt chart built: '" << data.name_ << "', " << n << " periods, timeAxis=" << timeAxis;
    return chart;
}

}  // namespace chart
