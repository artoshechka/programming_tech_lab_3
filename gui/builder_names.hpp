/// @file builder_names.hpp
/// @brief Канонические имена построителей графиков (ключи фабрики, подписи кнопок, тултипы).
/// @author Artemenko Anton
/// @details Одни и те же строковые идентификаторы построителей используются в регистрации фабрики
///          (app_compositor), при выборе типа графика (mainwindow) и при подборе тултипа
///          (ui_strings). Чтобы они не расходились, имена заданы здесь в единственном месте.
#ifndef GUID_b1f7c2a9_3e64_4d08_9a51_6c2f8b3e7d40
#define GUID_b1f7c2a9_3e64_4d08_9a51_6c2f8b3e7d40

namespace gui::builders
{

/// @brief Линия времени.
inline constexpr const char* kLine = "Line";
/// @brief Столбчатая диаграмма.
inline constexpr const char* kBar = "Bar";
/// @brief Круговая диаграмма.
inline constexpr const char* kPie = "Pie";
/// @brief Диаграмма-таймлайн (Gantt-стиль).
inline constexpr const char* kGantt = "Gantt";
/// @brief Диаграмма с областями.
inline constexpr const char* kArea = "Area";

}  // namespace gui::builders

#endif  // GUID_b1f7c2a9_3e64_4d08_9a51_6c2f8b3e7d40
