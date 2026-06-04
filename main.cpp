/// @file main.cpp
/// @brief Точка входа GUI-приложения.
/// @author Artemenko Anton

#include <QApplication>
#include <gui/src/app_compositor.hpp>
#include <gui/src/mainwindow.hpp>

/// @brief Точка входа приложения.
/// @param[in] argc Количество аргументов командной строки.
/// @param[in] argv Массив аргументов командной строки.
/// @return Код возврата приложения.
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    gui::CreateMainWindow()->show();
    return app.exec();
}
