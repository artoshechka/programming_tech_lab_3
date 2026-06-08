/// @file ipalette.hpp
/// @brief Минимальный интерфейс палитры для покраски элементов графика.
/// @author Artemenko Anton

#ifndef GUID_2b9a6c41_4e5d_4d51_9f0a_3b75d3d8f1a8
#define GUID_2b9a6c41_4e5d_4d51_9f0a_3b75d3d8f1a8

class QColor;

namespace style
{

/// @brief Поставляет цвета по индексу элемента — без знания о конкретных series Qt.
/// @details Стиль реализует IPalette, builders запрашивают цвета по индексу серии/среза.
///          Это убирает qobject_cast по типам series в стилях и переносит ответственность
///          за раскраску туда, где известна структура series — в builder.
class IPalette
{
   public:
    /// @brief Виртуальный деструктор.
    virtual ~IPalette() = default;

    /// @brief Возвращает цвет для элемента по индексу.
    /// @param[in] index Индекс элемента (0..total-1).
    /// @param[in] total Общее число элементов; нужно стилям с градиентом (например, grayscale).
    /// @return Цвет элемента.
    virtual QColor ColorFor(int index, int total) const = 0;
};

}  // namespace style

#endif  // GUID_2b9a6c41_4e5d_4d51_9f0a_3b75d3d8f1a8
