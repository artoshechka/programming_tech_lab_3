#ifndef GUID_f6a7b8c9_d0e1_2345_fabc_456789012345
#define GUID_f6a7b8c9_d0e1_2345_fabc_456789012345

#include <style/ichart_style.hpp>

namespace style {

/// @brief Чёрно-белый стиль.
class GrayscaleStyle : public IChartStyle {
public:
    void Apply(QChart* chart) override;
};

} // namespace style

#endif // GUID_f6a7b8c9_d0e1_2345_fabc_456789012345
