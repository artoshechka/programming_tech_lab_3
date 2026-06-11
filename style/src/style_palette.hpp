/// @file style_palette.hpp
/// @brief Константы цветовых палитр и параметров оформления стилей.
/// @author Artemenko Anton
#ifndef GUID_a1b2c3d4_e5f6_4789_a0b1_c2d3e4f50617
#define GUID_a1b2c3d4_e5f6_4789_a0b1_c2d3e4f50617

#include <QColor>
#include <QList>

namespace style
{

/// @brief Базовая бренд-палитра (Tableau-подобная) для раскраски серий.
inline const QList<QColor> kBrandPalette = {
    {0x4e, 0x79, 0xa7}, {0xf2, 0x8e, 0x2b}, {0xe1, 0x57, 0x59}, {0x76, 0xb7, 0xb2},
    {0x59, 0xa1, 0x4f}, {0xed, 0xc9, 0x48}, {0xb0, 0x7a, 0xa1}, {0xff, 0x9d, 0xa7},
    {0x9c, 0x75, 0x5f}, {0xba, 0xb0, 0xac}, {0x43, 0x9a, 0xcc},
};

constexpr int kGrayBase = 60;    ///< Базовый уровень серого (самый тёмный оттенок).
constexpr int kGrayRange = 170;  ///< Диапазон осветления оттенка по индексу серии.

}  // namespace style

#endif  // GUID_a1b2c3d4_e5f6_4789_a0b1_c2d3e4f50617
