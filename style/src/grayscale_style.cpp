#include "grayscale_style.hpp"

#include <QtCharts/QChart>
#include <algorithm>

QT_CHARTS_USE_NAMESPACE

namespace style
{

QColor GrayscaleStyle::ColorFor(int index, int total) const
{
    const int gray = 60 + (index * 170) / std::max(total - 1, 1);
    return QColor(gray, gray, gray);
}

void GrayscaleStyle::Apply(QtCharts::QChart* chart)
{
    chart->setTheme(QChart::ChartThemeLight);
    chart->setBackgroundBrush(Qt::white);
}

}  // namespace style
