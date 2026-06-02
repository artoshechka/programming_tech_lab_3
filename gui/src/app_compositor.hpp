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
#include <database_module/src/sqlite_db_manager.hpp>
#include <ioc_container/IOC_Contaner.hpp>
#include <logger/logger_factory.hpp>

namespace gui {

inline MainWindow* CreateMainWindow(QWidget* parent = nullptr) {
    // IoC: singletons live as instances, parsers are auto-wired from their deps.
    ioc::container::IOCContainer ioc;
    ioc.RegisterInstance<logger::ILogger>(logger::GetLogger<logger::AppLoggerTag>());
    ioc.RegisterInstance<database::manager::IDatabaseManager,
                         database::manager::SqliteDBManager>();
    ioc.RegisterFactory<parser::JsonParser, parser::JsonParser,
                        logger::ILogger>();
    ioc.RegisterFactory<parser::DatabaseParser, parser::DatabaseParser,
                        logger::ILogger, database::manager::IDatabaseManager>();

    auto dbManager = ioc.GetObject<database::manager::IDatabaseManager>();

    auto parserRegistry = std::make_shared<parser::ParserRegistry>();
    parserRegistry->Register("json",   ioc.GetObject<parser::JsonParser>());
    parserRegistry->Register("sqlite", ioc.GetObject<parser::DatabaseParser>());

    BuilderFactory builders;
    builders["Pie"] = [] { return std::make_shared<chart::PieChartBuilder>(); };
    builders["Bar"] = [] { return std::make_shared<chart::BarChartBuilder>(); };

    StyleFactory styles;
    styles["Color"]     = [] { return std::make_shared<style::ColorStyle>(); };
    styles["Grayscale"] = [] { return std::make_shared<style::GrayscaleStyle>(); };

    return new MainWindow(std::move(builders), std::move(styles), parserRegistry, dbManager, parent);
}

} // namespace gui

#endif // GUID_d0e1f2a3_b4c5_6789_defa_890123456789
