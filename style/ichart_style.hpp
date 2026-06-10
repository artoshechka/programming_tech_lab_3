/// @file ichart_style.hpp
/// @brief Объявление интерфейса стиля оформления графика.
/// @author Artemenko Anton
#ifndef GUID_b2c3d4e5_f6a7_8901_bcde_f12345678901
#define GUID_b2c3d4e5_f6a7_8901_bcde_f12345678901

#include <style/ipalette.hpp>

// Опережающее объявление: интерфейс не тянет <QtCharts/QChart> в потребителей,
// которые видят только сигнатуру; полный тип нужен только реализациям (в .cpp).
namespace QtCharts
{
class QChart;
}

namespace style
{

/// @brief Интерфейс стиля оформления графика.
/// @details Совмещает две роли: палитру цветов (через IPalette) и общие настройки
///          графика (тема, фон, легенда) через Apply(). Покраска отдельных серий и
///          срезов — обязанность builder'ов, которые получают палитру через
///          BuilderOptions; стилю больше не нужно знать о конкретных типах series.
class IChartStyle : public IPalette
{
   public:
    /// @brief Применяет к графику настройки, общие для всех серий (тема, фон).
    /// @param[in,out] chart График, к которому применяется стиль.
    virtual void Apply(QtCharts::QChart* chart) = 0;
};

}  // namespace style

#endif  // GUID_b2c3d4e5_f6a7_8901_bcde_f12345678901
