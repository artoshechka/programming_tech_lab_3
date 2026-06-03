/// @file grayscale_style.hpp
/// @brief Объявление чёрно-белого стиля оформления графика.
/// @author Artemenko Anton
#ifndef GUID_f6a7b8c9_d0e1_2345_fabc_456789012345
#define GUID_f6a7b8c9_d0e1_2345_fabc_456789012345

#include <style/ichart_style.hpp>

namespace style {

/// @brief Чёрно-белый стиль.
class GrayscaleStyle : public IChartStyle {
public:
    /// @brief Применяет градации серого к сериям графика.
    /// @details Элементам серий назначаются оттенки серого, равномерно
    ///          распределённые по градиенту от тёмного к светлому.
    /// @param[in,out] chart График, к которому применяется стиль.
    void Apply(QChart* chart) override;
};

} // namespace style

#endif // GUID_f6a7b8c9_d0e1_2345_fabc_456789012345
