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
    virtual ~IDatabase() = default;

    /// @brief Открывает БД по пути.
    /// @return true при успехе.
    virtual bool Open(const std::string& path) = 0;

    /// @brief Закрывает соединение.
    virtual void Close() = 0;

    /// @brief Список таблиц в открытой БД.
    virtual std::vector<std::string> Tables() const = 0;

    /// @brief Выполняет SELECT и передаёт каждую строку через визитор.
    /// @param sql  SQL-запрос.
    /// @param rowFn  вызывается для каждой ячейки строки: (colName, value).
    ///              Переход к следующей строке — новая серия вызовов.
    virtual void Query(const std::string& sql,
                       std::function<void(const std::string& col, const std::string& val)> rowFn) = 0;
};
}  // namespace database
#endif  // GUID_d1ed2330_293b_4360_a5ef_0df669aec760
