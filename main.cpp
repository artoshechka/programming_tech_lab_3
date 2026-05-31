/// @file main.cpp
/// @brief Точка входа GUI-приложения.
/// @author Artemenko Anton

#include <QApplication>
#include <gui/src/app_compositor.hpp>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    gui::CreateMainWindow()->show();
    return app.exec();
}
