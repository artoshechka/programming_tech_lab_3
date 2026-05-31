#ifndef GUID_d0e1f2a3_b4c5_6789_defa_890123456789
#define GUID_d0e1f2a3_b4c5_6789_defa_890123456789

#include <gui/src/mainwindow.hpp>
#include <chart/src/pie_chart_builder.hpp>
#include <chart/src/bar_chart_builder.hpp>
#include <style/src/color_style.hpp>
#include <style/src/grayscale_style.hpp>
#include <parser/src/parser_registry.hpp>
#include <parser/src/json_parser.hpp>
#include <parser/src/database_parser.hpp>
#include <logger/logger_factory.hpp>

namespace gui {

/// @brief Собирает MainWindow с зарегистрированными зависимостями.
inline MainWindow* CreateMainWindow(QWidget* parent = nullptr) {
    auto logger = logger::GetLogger<logger::AppLoggerTag>();

    auto parserRegistry = std::make_shared<parser::ParserRegistry>();
    parserRegistry->Register("json",   std::make_shared<parser::JsonParser>(logger));
    parserRegistry->Register("sqlite", std::make_shared<parser::DatabaseParser>(logger));

    BuilderFactory builders;
    builders["Pie"] = [] { return std::make_shared<chart::PieChartBuilder>(); };
    builders["Bar"] = [] { return std::make_shared<chart::BarChartBuilder>(); };

    StyleFactory styles;
    styles["Color"]     = [] { return std::make_shared<style::ColorStyle>(); };
    styles["Grayscale"] = [] { return std::make_shared<style::GrayscaleStyle>(); };

    return new MainWindow(std::move(builders), std::move(styles), parserRegistry, parent);
}

} // namespace gui

#endif // GUID_d0e1f2a3_b4c5_6789_defa_890123456789
