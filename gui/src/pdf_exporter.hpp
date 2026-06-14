/// @file pdf_exporter.hpp
/// @brief Объявление экспортёра графика в PDF.
/// @author Artemenko Anton
#ifndef GUID_f3a4b5c6_d7e8_9012_3456_789abcdef012
#define GUID_f3a4b5c6_d7e8_9012_3456_789abcdef012

#include <gui/ichart_exporter.hpp>

namespace gui
{

/// @brief Экспортёр сцены графика в PDF (страница A4) через QPdfWriter.
class PdfExporter : public IChartExporter
{
   public:
    /// @copydoc IChartExporter::Export
    [[nodiscard]] bool Export(QGraphicsScene& scene, const QString& path) override;
    /// @copydoc IChartExporter::FileFilter
    [[nodiscard]] QString FileFilter() const override;
};

}  // namespace gui

#endif  // GUID_f3a4b5c6_d7e8_9012_3456_789abcdef012
