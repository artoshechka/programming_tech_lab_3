/// @file color_style.hpp
/// @brief Объявление цветного стиля оформления графика.
/// @author Artemenko Anton
#ifndef GUID_e5f6a7b8_c9d0_1234_efab_345678901234
#define GUID_e5f6a7b8_c9d0_1234_efab_345678901234

#include <style/ichart_style.hpp>

namespace style
{

/// @brief Цветной стиль — тема Qt по умолчанию.
class ColorStyle : public IChartStyle
{
   public:
    /// @brief Применяет цветовую палитру к сериям графика.
    /// @details Каждому сектору круговой и каждому набору столбчатой диаграммы
    ///          назначается цвет из встроенной палитры по циклическому индексу.
    /// @param[in,out] chart График, к которому применяется стиль.
    void Apply(QtCharts::QChart* chart) override;
};

}  // namespace style

#endif  // GUID_e5f6a7b8_c9d0_1234_efab_345678901234
