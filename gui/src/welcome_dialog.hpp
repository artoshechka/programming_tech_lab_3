/// @file welcome_dialog.hpp
/// @brief Стартовый диалог-приглашение выбрать рабочую папку
/// @author Artemenko Anton
#ifndef GUID_5e8a4c12_9d37_4b6f_8a21_3c7e0f9b2d64
#define GUID_5e8a4c12_9d37_4b6f_8a21_3c7e0f9b2d64

#include <QColor>
#include <QDialog>
#include <QString>

namespace gui
{

/// @brief Приветственный модальный диалог: приглашает выбрать папку с данными до запуска главного окна.
/// @details Показывается первым при старте приложения. Кнопка «Выбрать папку» открывает системный
///          диалог выбора каталога; при непустом выборе диалог закрывается с accept() и отдаёт путь
///          через selectedFolder(). Закрытие без выбора оставляет путь пустым — приложение откроется
///          без заданного корня.
class WelcomeDialog : public QDialog
{
    Q_OBJECT
   public:
    /// @brief Конструктор приветственного диалога.
    /// @param[in] accent Акцентный цвет приложения для оформления кнопки.
    /// @param[in] parent Родительский виджет (по умолчанию nullptr — центрируется по экрану).
    explicit WelcomeDialog(const QColor& accent, QWidget* parent = nullptr);

    /// @brief Папка, выбранная пользователем.
    /// @return Абсолютный путь к папке; пустая строка, если выбор не сделан.
    [[nodiscard]] QString selectedFolder() const
    {
        return selectedFolder_;
    }

   private:
    /// @brief Открывает системный диалог выбора папки; при непустом выборе принимает диалог.
    void chooseFolder();

    QString selectedFolder_;  ///< Выбранный пользователем путь к папке (пустой, пока не выбран).
};

}  // namespace gui

#endif  // GUID_5e8a4c12_9d37_4b6f_8a21_3c7e0f9b2d64
