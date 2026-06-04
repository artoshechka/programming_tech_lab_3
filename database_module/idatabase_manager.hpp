/// @file idatabase_manager.hpp
/// @brief Интерфейс менеджера соединений с БД
/// @author Artemenko Anton
#ifndef GUID_b0aafd48_44bd_4e55_9553_99ffb1b538f0
#define GUID_b0aafd48_44bd_4e55_9553_99ffb1b538f0

#include <database_module/idatabase.hpp>
#include <memory>
#include <string>

namespace database::manager
{
/// @brief Фабрика соединений IDatabase.
class IDatabaseManager
{
   public:
    /// @brief Виртуальный деструктор.
    virtual ~IDatabaseManager() = default;

    /// @brief Создаёт новое соединение с базой данных.
    /// @param[in] connectionName Уникальное имя Qt-соединения.
    /// @return Разделяемый указатель на созданное соединение IDatabase.
    /// @details Имя connectionName должно быть уникальным в пределах процесса:
    ///          оно используется для идентификации соединения внутри Qt SQL.
    virtual std::shared_ptr<database::IDatabase> Create(const std::string& connectionName) = 0;
};
}  // namespace database::manager

#endif  // GUID_b0aafd48_44bd_4e55_9553_99ffb1b538f0
