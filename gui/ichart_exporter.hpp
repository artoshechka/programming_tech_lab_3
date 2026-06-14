/// @file ichart_exporter.hpp
/// @brief Объявление интерфейса экспортёра сцены графика в файл.
/// @author Artemenko Anton
/// @details Каждый формат вывода (PDF/PNG/SVG) — отдельная реализация IChartExporter.
///          Реализации регистрируются в фабрике по расширению (app_compositor) так же,
///          как построители и стили; добавление нового формата не требует правок MainWindow.
#ifndef GUID_e2f3a4b5_c6d7_8901_2345_6789abcdef01
#define GUID_e2f3a4b5_c6d7_8901_2345_6789abcdef01

#include <QString>

// Опережающее объявление: интерфейс видит лишь сигнатуру; полный тип сцены нужен
// только реализациям (в .cpp), которые фактически её рендерят.
class QGraphicsScene;

namespace gui
{

/// @brief Интерфейс экспортёра сцены графика в файл.
class IChartExporter
{
   public:
    /// @brief Виртуальный деструктор.
    virtual ~IChartExporter() = default;

    /// @brief Рендерит сцену графика в файл выбранного формата.
    /// @param[in] scene Сцена с построенным графиком.
    /// @param[in] path Путь к создаваемому файлу.
    /// @return true при успешной записи; false при ошибке.
    [[nodiscard]] virtual bool Export(QGraphicsScene& scene, const QString& path) = 0;

    /// @brief Фильтр файлов для диалога сохранения (например, "PDF (*.pdf)").
    /// @return Строка-фильтр в формате Qt.
    [[nodiscard]] virtual QString FileFilter() const = 0;
};

}  // namespace gui

#endif  // GUID_e2f3a4b5_c6d7_8901_2345_6789abcdef01
