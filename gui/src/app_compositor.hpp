#ifndef GUID_d0e1f2a3_b4c5_6789_defa_890123456789
#define GUID_d0e1f2a3_b4c5_6789_defa_890123456789

#include <ioc_container/IOC_Contaner.hpp>
#include <chart/ichart_builder.hpp>
#include <style/ichart_style.hpp>
#include <parser/iparser_registry.hpp>
#include <chart/src/pie_chart_builder.hpp>
#include <chart/src/bar_chart_builder.hpp>
#include <style/src/color_style.hpp>
#include <style/src/grayscale_style.hpp>
#include <parser/src/parser_registry.hpp>
#include <parser/src/json_parser.hpp>
#include <parser/src/database_parser.hpp>
#include <logger/logger_factory.hpp>

namespace gui {

/// @brief Регистрирует все зависимости в IoC-контейнере.
inline void RegisterDependencies(ioc::container::IOCContainer& ioc) {
    auto logger = logger::GetLogger<logger::AppLoggerTag>();
    ioc.RegisterInstance<logger::ILogger>(logger);

    auto registry = std::make_shared<parser::ParserRegistry>();
    registry->Register("json",   std::make_shared<parser::JsonParser>(logger));
    registry->Register("sqlite", std::make_shared<parser::DatabaseParser>(logger));
    ioc.RegisterInstance<parser::IParserRegistry>(registry);

    ioc.RegisterFactory<chart::IChartBuilder, chart::PieChartBuilder>();
    ioc.RegisterFactory<style::IChartStyle,   style::ColorStyle>();
}

} // namespace gui

#endif // GUID_d0e1f2a3_b4c5_6789_defa_890123456789
