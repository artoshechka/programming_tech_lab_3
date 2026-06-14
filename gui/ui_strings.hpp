/// @file ui_strings.hpp
/// @brief Централизованные user-facing строки интерфейса (метки, заголовки, тексты диалогов).
/// @author Artemenko Anton
/// @details Здесь собраны только строки, видимые пользователю в GUI: подписи кнопок,
///          заголовки окон и диалогов, тексты сообщений об ошибках, фильтры файлов.
///          Диагностические сообщения для лога сюда НЕ входят — они англоязычные и
///          остаются рядом с кодом, который их пишет. Все константы имеют внешнюю
///          компоновку (inline constexpr); в местах вызова приводятся к QString через конструктор.
#ifndef GUID_a7c1e93b_5d24_4f80_9b3a_1c6e2f8a4d05
#define GUID_a7c1e93b_5d24_4f80_9b3a_1c6e2f8a4d05

#include <gui/builder_names.hpp>
#include <string_view>

namespace gui::ui
{

/// @brief Заголовок панели инструментов.
inline constexpr const char* kToolbarTitle = "Controls";

/// @brief Тултип кнопки «Line»: как формируется линия времени.
inline constexpr const char* kLineTooltip =
    "Линия времени: все точки value(t) на реальной оси времени в хронологическом "
    "порядке, без агрегации — сохраняет суть временного ряда.";
/// @brief Тултип кнопки «Bar»: как формируется столбчатая диаграмма.
inline constexpr const char* kBarTooltip =
    "Столбчатая диаграмма: при числе точек > 50 значения усредняются по месяцу "
    "(MM.YYYY); высота столбца — среднее за период.";
/// @brief Тултип кнопки «Pie»: как формируется круговая диаграмма.
inline constexpr const char* kPieTooltip =
    "Круговая диаграмма: при числе точек > 50 — группировка по месяцу (MM.YYYY); "
    "показываются 10 крупнейших долей, остальное сводится в сектор «Other».";
/// @brief Тултип кнопки «Gantt»: как формируется диаграмма-таймлайн.
inline constexpr const char* kGanttTooltip =
    "Диаграмма-таймлайн: ряд группируется по месяцу; каждый период — горизонтальная "
    "полоса, смещённая по реальной оси времени (ступеньками) и окрашенная палитрой.";
/// @brief Тултип кнопки «Area»: как формируется диаграмма с областями.
inline constexpr const char* kAreaTooltip =
    "Диаграмма с областями: все точки value(t) на реальной оси времени с заливкой "
    "площади под кривой — подчёркивает форму, объём и динамику ряда, без агрегации.";

/// @brief Возвращает тултип-описание для построителя по его имени.
/// @param[in] builderName Имя построителя ("Line"/"Bar"/"Pie"/"Gantt"/"Area").
/// @return Текст описания; пустая строка, если построитель неизвестен.
inline const char* TooltipForBuilder(std::string_view builderName)
{
    if (builderName == builders::kLine) return kLineTooltip;
    if (builderName == builders::kBar) return kBarTooltip;
    if (builderName == builders::kPie) return kPieTooltip;
    if (builderName == builders::kGantt) return kGanttTooltip;
    if (builderName == builders::kArea) return kAreaTooltip;
    return "";
}

/// @brief Подпись кнопки выбора палитры.
inline constexpr const char* kPaletteButton = " Палитра";
/// @brief Заголовок поповера выбора палитры.
inline constexpr const char* kPaletteHeader = "Палитра";
/// @brief Имя палитры/стиля по умолчанию (оранжевая — в стилистике приложения).
inline constexpr const char* kDefaultStyleName = "Тёплая";
/// @brief Подпись кнопки выбора рабочей папки.
inline constexpr const char* kFolderButton = "Папка";
/// @brief Подпись кнопки сохранения графика в файл.
inline constexpr const char* kSaveButton = "Сохранить";
/// @brief Подпись кнопки переключения светлой темы.
inline constexpr const char* kThemeLightButton = "☀ Светлая";
/// @brief Подпись кнопки переключения тёмной темы.
inline constexpr const char* kThemeDarkButton = "☾ Тёмная";
/// @brief Текст-заглушка заголовка графика, пока данные не загружены.
inline constexpr const char* kPlotTitlePlaceholder = "Выберите файл с данными";
/// @brief Суффикс счётчика точек ряда в статус-баре.
inline constexpr const char* kPointsSuffix = " точек";

/// @brief Заголовок диалога выбора рабочей папки с данными.
inline constexpr const char* kChooseFolderTitle = "Выберите папку с данными";
/// @brief Заголовок диалога сохранения графика в файл.
inline constexpr const char* kSaveChartDialogTitle = "Сохранить график";
/// @brief Фильтр файлов диалога сохранения PDF.
inline constexpr const char* kPdfFilter = "PDF (*.pdf)";
/// @brief Фильтр файлов диалога сохранения PNG.
inline constexpr const char* kPngFilter = "PNG (*.png)";
/// @brief Заголовок диалога выбора таблицы SQLite.
inline constexpr const char* kSelectTableTitle = "Выберите таблицу";

/// @brief Заголовок окна сообщения об общей ошибке.
inline constexpr const char* kErrorTitle = "Ошибка";
/// @brief Заголовок окна сообщения об ошибке загрузки данных.
inline constexpr const char* kLoadErrorTitle = "Ошибка загрузки";
/// @brief Префикс сообщения статус-бара об успешном сохранении.
inline constexpr const char* kSavedPrefix = "Сохранено: ";
/// @brief Текст ошибки при неудачном сохранении графика в файл.
inline constexpr const char* kSaveErrorMessage = "Не удалось сохранить график в файл.";

/// @brief Заголовок окна фатальной ошибки при старте приложения.
inline constexpr const char* kFatalErrorTitle = "Fatal error";
/// @brief Текст сообщения о неизвестном исключении при старте приложения.
inline constexpr const char* kUnknownStartupError = "Unknown exception during application startup.";

}  // namespace gui::ui

#endif  // GUID_a7c1e93b_5d24_4f80_9b3a_1c6e2f8a4d05
