/// @file ichart_style.hpp
/// @brief Объявление интерфейса стиля оформления графика.
/// @author Artemenko Anton
#ifndef GUID_b2c3d4e5_f6a7_8901_bcde_f12345678901
#define GUID_b2c3d4e5_f6a7_8901_bcde_f12345678901

#include <QtCharts/QChart>

QT_CHARTS_USE_NAMESPACE

namespace style {

/// @brief Интерфейс стиля оформления графика.
class IChartStyle {
public:
    /// @brief Виртуальный деструктор.
    virtual ~IChartStyle() = default;

    /// @brief Применяет стиль к переданному графику.
    /// @param[in,out] chart График, к которому применяется стиль.
    virtual void Apply(QChart* chart) = 0;
};

} // namespace style

#endif // GUID_b2c3d4e5_f6a7_8901_bcde_f12345678901
