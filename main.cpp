/// @file main.cpp
/// @brief Точка входа GUI-приложения.
/// @author Artemenko Anton

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <exception>
#include <gui/app_compositor.hpp>
#include <gui/mainwindow.hpp>
#include <gui/ui_strings.hpp>
#include <logger/logger_factory.hpp>
#include <logger/logger_macros.hpp>
#include <memory>

/// @brief Точка входа приложения.
/// @param[in] argc Количество аргументов командной строки.
/// @param[in] argv Массив аргументов командной строки.
/// @return Код возврата приложения; 1 — std::exception на старте, 2 — неизвестное исключение.
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    auto logger = logger::GetLogger<logger::AppLoggerTag>();
    try
    {
        std::unique_ptr<gui::MainWindow> window(gui::CreateMainWindow());
        window->show();
        return app.exec();
    } catch (const std::exception& ex)
    {
        LogFatal(logger) << "Fatal std::exception during startup: " << ex.what();
        QMessageBox::critical(nullptr, gui::ui::kFatalErrorTitle, QString::fromUtf8(ex.what()));
        return 1;
    } catch (...)
    {
        LogFatal(logger) << "Fatal unknown exception during startup";
        QMessageBox::critical(nullptr, gui::ui::kFatalErrorTitle, gui::ui::kUnknownStartupError);
        return 2;
    }
}
