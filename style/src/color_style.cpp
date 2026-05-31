#include "color_style.hpp"

QT_CHARTS_USE_NAMESPACE

namespace style {

void ColorStyle::Apply(QChart* chart) {
    chart->setTheme(QChart::ChartThemeLight);
}

} // namespace style
