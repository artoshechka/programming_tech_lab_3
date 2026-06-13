/// @file render_constants.hpp
/// @brief Константы отрисовки графиков (приватные детали реализации builder'ов).
/// @author Artemenko Anton
#ifndef GUID_c3d4e5f6_0718_4912_c3d4_e5f607182939
#define GUID_c3d4e5f6_0718_4912_c3d4_e5f607182939

namespace chart
{

constexpr int kMaxSlices = 10;  ///< Максимум отображаемых секторов круговой диаграммы.

constexpr double kAxisPaddingFraction = 0.15;  ///< Доля диапазона значений как вертикальный отступ оси Y.
constexpr int kDateAxisTicks = 8;              ///< Число делений временной оси (QDateTimeAxis).
constexpr int kValueAxisTicks = 6;             ///< Число делений оси значений (QValueAxis).
constexpr double kSeriesLineWidth = 1.6;       ///< Толщина линии ряда (линия / контур области).
constexpr int kAreaFillAlpha = 110;            ///< Прозрачность заливки области под кривой (0..255).

constexpr const char* kDayAxisFormat = "dd.MM.yyyy";         ///< Формат подписей дневной временной оси.
constexpr const char* kMonthAxisFormat = "MM.yyyy";          ///< Формат подписей помесячной временной оси.
constexpr const char* kTimeFormatFull = "dd.MM.yyyy HH:mm";  ///< Полный формат разбора метки времени.
constexpr const char* kTimeFormatDay = "dd.MM.yyyy";         ///< Дневной формат разбора метки времени (фолбэк).

}  // namespace chart

#endif  // GUID_c3d4e5f6_0718_4912_c3d4_e5f607182939
