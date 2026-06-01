#include "grayscale_style.hpp"
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>

QT_CHARTS_USE_NAMESPACE

namespace style {

void GrayscaleStyle::Apply(QChart* chart) {
    chart->setTheme(QChart::ChartThemeLight);
    chart->setBackgroundBrush(Qt::white);

    for (auto* series : chart->series()) {
        if (auto* pie = qobject_cast<QPieSeries*>(series)) {
            auto slices = pie->slices();
            const int n = slices.size();
            for (int i = 0; i < n; ++i) {
                int gray = 60 + (i * 170) / std::max(n - 1, 1);
                slices[i]->setColor(QColor(gray, gray, gray));
                slices[i]->setBorderColor(Qt::white);
            }
        } else if (auto* bar = qobject_cast<QBarSeries*>(series)) {
            auto sets = bar->barSets();
            const int n = sets.size();
            for (int i = 0; i < n; ++i) {
                int gray = 80 + (i * 140) / std::max(n - 1, 1);
                sets[i]->setColor(QColor(gray, gray, gray));
            }
        }
    }
}

} // namespace style
