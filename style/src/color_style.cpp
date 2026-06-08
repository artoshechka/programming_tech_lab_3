#include "color_style.hpp"

#include <QtCharts/QAbstractAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

QT_CHARTS_USE_NAMESPACE

namespace style
{

static const QList<QColor> kPalette = {
    {0x4e, 0x79, 0xa7}, {0xf2, 0x8e, 0x2b}, {0xe1, 0x57, 0x59}, {0x76, 0xb7, 0xb2},
    {0x59, 0xa1, 0x4f}, {0xed, 0xc9, 0x48}, {0xb0, 0x7a, 0xa1}, {0xff, 0x9d, 0xa7},
    {0x9c, 0x75, 0x5f}, {0xba, 0xb0, 0xac}, {0x43, 0x9a, 0xcc},
};

void ColorStyle::Apply(QtCharts::QChart* chart)
{
    for (auto* series : chart->series())
    {
        if (auto* pie = qobject_cast<QPieSeries*>(series))
        {
            auto slices = pie->slices();
            for (int i = 0; i < slices.size(); ++i)
            {
                slices[i]->setColor(kPalette[i % kPalette.size()]);
                slices[i]->setBorderColor(Qt::white);
                slices[i]->setLabelColor(Qt::black);
            }
        } else if (auto* bar = qobject_cast<QBarSeries*>(series))
        {
            auto sets = bar->barSets();
            for (int i = 0; i < sets.size(); ++i) sets[i]->setColor(kPalette[i % kPalette.size()]);
        }
    }
}

}  // namespace style
