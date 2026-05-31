#include "grayscale_style.hpp"

QT_CHARTS_USE_NAMESPACE

namespace style {

void GrayscaleStyle::Apply(QChart* chart) {
    chart->setTheme(QChart::ChartThemeHighContrast);
}

} // namespace style
