#include "color_style.hpp"

#include <QList>
#include <QtCharts/QChart>
#include <logger/logger_macros.hpp>

QT_CHARTS_USE_NAMESPACE

namespace style
{

static const QList<QColor> kPalette = {
    {0x4e, 0x79, 0xa7}, {0xf2, 0x8e, 0x2b}, {0xe1, 0x57, 0x59}, {0x76, 0xb7, 0xb2},
    {0x59, 0xa1, 0x4f}, {0xed, 0xc9, 0x48}, {0xb0, 0x7a, 0xa1}, {0xff, 0x9d, 0xa7},
    {0x9c, 0x75, 0x5f}, {0xba, 0xb0, 0xac}, {0x43, 0x9a, 0xcc},
};

QColor ColorStyle::ColorFor(int index, int total) const
{
    (void)total;
    return kPalette[index % kPalette.size()];
}

void ColorStyle::Apply(QtCharts::QChart* chart)
{
    // Покраска серий идёт через ColorFor() в builder'ах; здесь только общая тема.
    chart->setTheme(QChart::ChartThemeLight);
    LogInfo(logger_) << "ColorStyle applied: light theme, " << kPalette.size() << "-color palette";
}

}  // namespace style
