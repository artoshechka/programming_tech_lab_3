/// @file idata.hpp
/// @brief Интерфейс данных
/// @author Artemenko Anton
#ifndef GUID_47aad79f_00cb_4717_8578_185d1fa9915b
#define GUID_47aad79f_00cb_4717_8578_185d1fa9915b

namespace data
{
/// @brief Интерфейс данных.
class IData
{
   public:
    /// @brief Виртуальный деструктор интерфейса.
    virtual ~IData() = default;

    /// @brief Метод для получения данных.
    virtual void GetData() = 0;
};
}  // namespace data
#endif  // GUID_47aad79f_00cb_4717_8578_185d1fa9915b