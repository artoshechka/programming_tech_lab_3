/// @file table_select_dialog.hpp
/// @brief Диалог выбора таблицы SQLite
/// @author Artemenko Anton
#ifndef GUID_c2d3e4f5_a6b7_8901_cdef_012345678901
#define GUID_c2d3e4f5_a6b7_8901_cdef_012345678901

#include <QDialog>
#include <QListWidget>
#include <QStringList>

namespace gui {

/// @brief Показывает список таблиц и возвращает выбранную.
class TableSelectDialog : public QDialog {
    Q_OBJECT
public:
    explicit TableSelectDialog(const QStringList& tables, QWidget* parent = nullptr);

    /// @return Выбранное имя таблицы или пустую строку если отменено.
    QString selectedTable() const;

private:
    QListWidget* list_;
};

} // namespace gui

#endif // GUID_c2d3e4f5_a6b7_8901_cdef_012345678901
