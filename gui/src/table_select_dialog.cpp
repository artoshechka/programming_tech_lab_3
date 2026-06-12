/// @file table_select_dialog.cpp
/// @brief Определение TableSelectDialog
/// @author Artemenko Anton

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <gui/src/table_select_dialog.hpp>
#include <gui/ui_strings.hpp>

namespace gui
{

TableSelectDialog::TableSelectDialog(const QStringList& tables, QWidget* parent)
    : QDialog(parent), list_(new QListWidget(this))
{
    setWindowTitle(ui::kSelectTableTitle);
    list_->addItems(tables);
    if (!tables.isEmpty()) list_->setCurrentRow(0);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(list_);
    layout->addWidget(buttons);
}

QString TableSelectDialog::selectedTable() const
{
    auto* item = list_->currentItem();
    return item ? item->text() : QString{};
}

}  // namespace gui
