/// @file main.cpp
/// @brief Точка входа GUI-приложения.
/// @author Artemenko Anton

#include <QApplication>
#include <gui/src/app_compositor.hpp>
#include <gui/src/mainwindow.hpp>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    ioc::container::IOCContainer ioc;
    gui::RegisterDependencies(ioc);

    gui::MainWindow window(
        ioc.GetObject<chart::IChartBuilder>(),
        ioc.GetObject<style::IChartStyle>(),
        ioc.GetObject<parser::IParserRegistry>()
    );
    window.show();

    return app.exec();
}
