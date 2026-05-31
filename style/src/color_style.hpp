#ifndef GUID_e5f6a7b8_c9d0_1234_efab_345678901234
#define GUID_e5f6a7b8_c9d0_1234_efab_345678901234

#include <style/ichart_style.hpp>

namespace style {

/// @brief Цветной стиль — тема Qt по умолчанию.
class ColorStyle : public IChartStyle {
public:
    void Apply(QChart* chart) override;
};

} // namespace style

#endif // GUID_e5f6a7b8_c9d0_1234_efab_345678901234
