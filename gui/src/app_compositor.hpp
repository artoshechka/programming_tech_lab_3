/// @file app_compositor.hpp
/// @brief Сборка зависимостей приложения и создание главного окна
/// @author Artemenko Anton
#ifndef GUID_d0e1f2a3_b4c5_6789_defa_890123456789
#define GUID_d0e1f2a3_b4c5_6789_defa_890123456789

#include <QWidget>

namespace gui
{

class MainWindow;

/// @brief Создаёт и связывает все зависимости приложения, возвращая готовое главное окно.
/// @details Точка композиции (IoC): регистрирует логгер, менеджер БД и парсеры в контейнере,
///          собирает реестр парсеров и фабрики построителей/стилей, внедряет их в MainWindow.
/// @param[in] parent Родительский виджет (по умолчанию nullptr).
/// @return Указатель на созданное главное окно (владение передаётся вызывающему).
MainWindow* CreateMainWindow(QWidget* parent = nullptr);

}  // namespace gui

#endif  // GUID_d0e1f2a3_b4c5_6789_defa_890123456789
