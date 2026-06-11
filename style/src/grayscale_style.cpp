#include "grayscale_style.hpp"

#include <QtCharts/QChart>
#include <algorithm>
#include <logger/logger_macros.hpp>
#include <style/src/style_palette.hpp>

QT_CHARTS_USE_NAMESPACE

namespace style
{

QColor GrayscaleStyle::ColorFor(int index, int total) const
{
    const int gray = kGrayBase + (index * kGrayRange) / std::max(total - 1, 1);
    return QColor(gray, gray, gray);
}

void GrayscaleStyle::Apply(QtCharts::QChart* chart)
{
    chart->setTheme(QChart::ChartThemeLight);
    chart->setBackgroundBrush(Qt::white);
    LogInfo(logger_) << "GrayscaleStyle applied: light theme, white background";
}

}  // namespace style
