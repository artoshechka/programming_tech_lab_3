/// @file idatabase.hpp
/// @brief Интерфейс соединения с базой данных
/// @author Artemenko Anton
#ifndef GUID_d1ed2330_293b_4360_a5ef_0df669aec760
#define GUID_d1ed2330_293b_4360_a5ef_0df669aec760

#include <functional>
#include <string>
#include <vector>

namespace database
{
/// @brief Интерфейс соединения с одной базой данных.
class IDatabase
{
   public:
    /// @brief Виртуальный деструктор.
    virtual ~IDatabase() = default;

    /// @brief Открывает базу данных по указанному пути.
    /// @param[in] path Путь к файлу базы данных.
    /// @return true при успешном открытии, иначе false.
    virtual bool Open(const std::string& path) = 0;

    /// @brief Закрывает текущее соединение с базой данных.
    virtual void Close() = 0;

    /// @brief Возвращает список таблиц в открытой базе данных.
    /// @return Вектор имён таблиц.
    virtual std::vector<std::string> Tables() const = 0;

    /// @brief Выполняет SQL-запрос и передаёт каждую ячейку результата через визитор.
    /// @param[in] sql Текст SQL-запроса.
    /// @param[in] rowFn Визитор, вызываемый для каждой ячейки строки результата.
    /// @details Сигнатура визитора: void(const std::string& col, const std::string& val),
    ///          где col — имя столбца, val — строковое представление значения ячейки.
    ///          Для каждой строки результата выполняется серия вызовов по всем её столбцам;
    ///          переход к следующей строке начинает новую серию вызовов.
    virtual void Query(const std::string& sql,
                       std::function<void(const std::string& col, const std::string& val)> rowFn) = 0;
};
}  // namespace database
#endif  // GUID_d1ed2330_293b_4360_a5ef_0df669aec760
