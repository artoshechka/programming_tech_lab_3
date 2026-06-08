/// @file grayscale_style.hpp
/// @brief Объявление чёрно-белого стиля оформления графика.
/// @author Artemenko Anton
#ifndef GUID_f6a7b8c9_d0e1_2345_fabc_456789012345
#define GUID_f6a7b8c9_d0e1_2345_fabc_456789012345

#include <QColor>
#include <style/ichart_style.hpp>

namespace style
{

/// @brief Чёрно-белый стиль с равномерным градиентом серого.
class GrayscaleStyle : public IChartStyle
{
   public:
    /// @brief Возвращает оттенок серого по индексу элемента.
    /// @param[in] index Индекс элемента (0..total-1).
    /// @param[in] total Общее число элементов; определяет шаг градиента.
    /// @return Цвет градации серого.
    QColor ColorFor(int index, int total) const override;

    /// @brief Применяет к графику светлую тему и белый фон; покраска идёт через ColorFor().
    /// @param[in,out] chart График, к которому применяется стиль.
    void Apply(QtCharts::QChart* chart) override;
};

}  // namespace style

#endif  // GUID_f6a7b8c9_d0e1_2345_fabc_456789012345
