/// @file app_compositor.cpp
/// @brief Реализация точки композиции приложения
/// @author Artemenko Anton

#include <chart/src/area_chart_builder.hpp>
#include <chart/src/bar_chart_builder.hpp>
#include <chart/src/gantt_chart_builder.hpp>
#include <chart/src/line_chart_builder.hpp>
#include <chart/src/pie_chart_builder.hpp>
#include <database_module/src/sqlite_db_manager.hpp>
#include <gui/app_compositor.hpp>
#include <gui/builder_names.hpp>
#include <gui/mainwindow.hpp>
#include <ioc_container/IOC_Contaner.hpp>
#include <logger/logger_factory.hpp>
#include <logger/logger_macros.hpp>
#include <parser/src/database_parser.hpp>
#include <parser/src/json_parser.hpp>
#include <parser/src/parser_registry.hpp>
#include <style/src/color_style.hpp>
#include <style/src/grayscale_style.hpp>
#include <style/src/style_palette.hpp>

namespace gui
{

MainWindow* CreateMainWindow(QWidget* parent)
{
    // IoC: singletons live as instances, parsers are auto-wired from their deps.
    auto appLogger = logger::GetLogger<logger::AppLoggerTag>();
    LogInfo(appLogger) << "Composition root: wiring application";

    ioc::container::IOCContainer ioc;
    ioc.RegisterInstance<logger::ILogger>(appLogger);
    ioc.RegisterInstance<database::manager::IDatabaseManager, database::manager::SqliteDBManager, logger::ILogger>();
    ioc.RegisterFactory<parser::JsonParser, parser::JsonParser, logger::ILogger>();
    ioc.RegisterFactory<parser::DatabaseParser, parser::DatabaseParser, logger::ILogger,
                        database::manager::IDatabaseManager>();

    // Декларативный список парсеров: одна строка на формат, как и в Builder/StyleFactory.
    ParserFactory parsers;
    parsers["json"] = [&ioc] { return ioc.GetObject<parser::JsonParser>(); };
    parsers["sqlite"] = [&ioc] { return ioc.GetObject<parser::DatabaseParser>(); };

    auto parserRegistry = std::make_shared<parser::ParserRegistry>(appLogger);
    for (const auto& [ext, factory] : parsers)
    {
        auto p = factory();
        if (!p) LogError(appLogger) << "IoC: failed to resolve parser for '" << ext << "'";
        parserRegistry->Register(ext, std::move(p));
    }

    // Билдеры и стили получают логгер через захват в фабриках.
    BuilderFactory builders;
    builders[::gui::builders::kLine] = [appLogger] { return std::make_shared<chart::LineChartBuilder>(appLogger); };
    builders[::gui::builders::kArea] = [appLogger] { return std::make_shared<chart::AreaChartBuilder>(appLogger); };
    builders[::gui::builders::kGantt] = [appLogger] { return std::make_shared<chart::GanttChartBuilder>(appLogger); };
    builders[::gui::builders::kPie] = [appLogger] { return std::make_shared<chart::PieChartBuilder>(appLogger); };
    builders[::gui::builders::kBar] = [appLogger] { return std::make_shared<chart::BarChartBuilder>(appLogger); };

    StyleFactory styles;
    styles["Тёплая"] = [appLogger] { return std::make_shared<style::ColorStyle>(style::kWarmPalette, appLogger); };
    styles["Контраст"] = [appLogger] { return std::make_shared<style::ColorStyle>(style::kContrastPalette, appLogger); };
    styles["Графит"] = [appLogger] { return std::make_shared<style::ColorStyle>(style::kGraphitePalette, appLogger); };
    styles["Индиго"] = [appLogger] { return std::make_shared<style::ColorStyle>(style::kIndigoPalette, appLogger); };
    styles["Океан"] = [appLogger] { return std::make_shared<style::ColorStyle>(style::kOceanPalette, appLogger); };
    styles["Оттенки серого"] = [appLogger] { return std::make_shared<style::GrayscaleStyle>(appLogger); };

    LogInfo(appLogger) << "Composition root: ready (" << parserRegistry->SupportedExtensions().size() << " parsers, "
                       << builders.size() << " builders, " << styles.size() << " styles)";
    return new MainWindow(std::move(builders), std::move(styles), parserRegistry, appLogger, parent);
}

}  // namespace gui
