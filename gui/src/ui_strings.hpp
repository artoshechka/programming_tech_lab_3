/// @file ui_strings.hpp
/// @brief Централизованные user-facing строки интерфейса (метки, заголовки, тексты диалогов).
/// @author Artemenko Anton
/// @details Здесь собраны только строки, видимые пользователю в GUI: подписи кнопок,
///          заголовки окон и диалогов, тексты сообщений об ошибках, фильтры файлов.
///          Диагностические сообщения для лога сюда НЕ входят — они англоязычные и
///          остаются рядом с кодом, который их пишет. Все константы имеют внешнюю
///          компоновку (inline constexpr) и неявно приводятся к QString в местах вызова.
#ifndef GUID_a7c1e93b_5d24_4f80_9b3a_1c6e2f8a4d05
#define GUID_a7c1e93b_5d24_4f80_9b3a_1c6e2f8a4d05

namespace gui::ui
{

/// @brief Заголовок панели инструментов.
inline constexpr const char* kToolbarTitle = "Controls";
/// @brief Подпись чекбокса включения агрегации.
inline constexpr const char* kAggregateCheckbox = "Агрегация";
/// @brief Подпись кнопки выбора рабочей папки.
inline constexpr const char* kFolderButton = "Папка";
/// @brief Подпись кнопки сохранения графика в PDF.
inline constexpr const char* kSavePdfButton = "Сохранить PDF";

/// @brief Заголовок диалога выбора рабочей папки с данными.
inline constexpr const char* kChooseFolderTitle = "Выберите папку с данными";
/// @brief Заголовок диалога сохранения PDF.
inline constexpr const char* kSavePdfDialogTitle = "Сохранить PDF";
/// @brief Фильтр файлов диалога сохранения PDF.
inline constexpr const char* kPdfFilter = "PDF (*.pdf)";
/// @brief Заголовок диалога выбора таблицы SQLite.
inline constexpr const char* kSelectTableTitle = "Выберите таблицу";

/// @brief Заголовок окна сообщения об общей ошибке.
inline constexpr const char* kErrorTitle = "Ошибка";
/// @brief Заголовок окна сообщения об ошибке загрузки данных.
inline constexpr const char* kLoadErrorTitle = "Ошибка загрузки";
/// @brief Префикс сообщения статус-бара об успешном сохранении.
inline constexpr const char* kSavedPrefix = "Сохранено: ";

/// @brief Заголовок окна фатальной ошибки при старте приложения.
inline constexpr const char* kFatalErrorTitle = "Fatal error";
/// @brief Текст сообщения о неизвестном исключении при старте приложения.
inline constexpr const char* kUnknownStartupError = "Unknown exception during application startup.";

}  // namespace gui::ui

#endif  // GUID_a7c1e93b_5d24_4f80_9b3a_1c6e2f8a4d05
