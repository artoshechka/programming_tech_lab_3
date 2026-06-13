/// @file file_glyphs.hpp
/// @brief Векторные глифы иконок файлов/папок и классификация элемента по расширению.
/// @author Artemenko Anton
/// @details Иконки рисуются вектором в канве 16×16 и масштабируются под целевой прямоугольник,
///          что даёт DPR-резкость без растровых ресурсов. Модуль общий для делегата фона и
///          живого виджета строки дерева.
#ifndef GUID_2b6f4c08_9e51_4a73_8c2d_5f1a90b6e4d7
#define GUID_2b6f4c08_9e51_4a73_8c2d_5f1a90b6e4d7

#include <QColor>
#include <QString>

class QPainter;
class QRectF;

namespace gui
{

/// @brief Тип файла для выбора иконки в дереве.
enum class FileKind
{
    Folder,   ///< Папка.
    Json,     ///< JSON-файл.
    Sqlite,   ///< База SQLite.
    Generic,  ///< Любой другой файл.
};

/// @brief Классифицирует элемент дерева по признаку папки и расширению.
/// @param[in] isDir Является ли элемент папкой.
/// @param[in] suffixLower Расширение файла в нижнем регистре (без точки); для папок игнорируется.
/// @return Тип файла для выбора иконки.
FileKind ClassifyFileKind(bool isDir, const QString& suffixLower);

/// @brief Рисует контурный глиф иконки в заданном прямоугольнике.
/// @param[in] painter Целевой painter (состояние сохраняется и восстанавливается внутри).
/// @param[in] rect Прямоугольник отрисовки (квадратный; глиф масштабируется по его ширине).
/// @param[in] kind Тип файла.
/// @param[in] color Цвет контура.
void PaintFileGlyph(QPainter* painter, const QRectF& rect, FileKind kind, const QColor& color);

}  // namespace gui

#endif  // GUID_2b6f4c08_9e51_4a73_8c2d_5f1a90b6e4d7
