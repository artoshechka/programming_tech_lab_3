#include "color_style.hpp"

#include <QtCharts/QChart>
#include <logger/logger_macros.hpp>

QT_CHARTS_USE_NAMESPACE

namespace style
{

QColor ColorStyle::ColorFor(int index, int total) const
{
    (void)total;
    return palette_[index % palette_.size()];
}

void ColorStyle::Apply(QtCharts::QChart* chart)
{
    // Покраска серий идёт через ColorFor() в builder'ах; здесь только общая тема.
    chart->setTheme(QChart::ChartThemeLight);
    LogInfo(logger_) << "ColorStyle applied: light theme, " << palette_.size() << "-color palette";
}

}  // namespace style
