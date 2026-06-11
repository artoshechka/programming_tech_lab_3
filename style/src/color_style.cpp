#include "color_style.hpp"

#include <QtCharts/QChart>
#include <logger/logger_macros.hpp>
#include <style/src/style_palette.hpp>

QT_CHARTS_USE_NAMESPACE

namespace style
{

QColor ColorStyle::ColorFor(int index, int total) const
{
    (void)total;
    return kBrandPalette[index % kBrandPalette.size()];
}

void ColorStyle::Apply(QtCharts::QChart* chart)
{
    // Покраска серий идёт через ColorFor() в builder'ах; здесь только общая тема.
    chart->setTheme(QChart::ChartThemeLight);
    LogInfo(logger_) << "ColorStyle applied: light theme, " << kBrandPalette.size() << "-color palette";
}

}  // namespace style
