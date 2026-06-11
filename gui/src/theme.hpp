/// @file theme.hpp
/// @brief Светлая и тёмная темы оформления интерфейса (Qt Style Sheets, оранжевый акцент).
/// @author Artemenko Anton
#ifndef GUID_e5f60718_293a_4b14_e5f6_0718293a4b5c
#define GUID_e5f60718_293a_4b14_e5f6_0718293a4b5c

#include <QString>

namespace gui::theme
{

/// @brief Режим оформления интерфейса.
enum class Mode
{
    Light,  ///< Светлая тема.
    Dark    ///< Тёмная тема.
};

/// @brief Возвращает таблицу стилей (QSS) для указанного режима.
/// @param[in] mode Режим оформления.
/// @return Готовая строка QSS для установки в qApp->setStyleSheet().
QString StyleSheet(Mode mode);

}  // namespace gui::theme

#endif  // GUID_e5f60718_293a_4b14_e5f6_0718293a4b5c
