/// @file schema.hpp
/// @brief Обобщённый визитор полей структуры по статической схеме
/// @author Artemenko Anton
#ifndef GUID_b3f1d2c4_5a6e_4f8b_9c0d_1e2f3a4b5c6d
#define GUID_b3f1d2c4_5a6e_4f8b_9c0d_1e2f3a4b5c6d

#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace data
{
/// @brief Признак наличия статической схемы kSchema у типа
/// @tparam T Проверяемый тип
template <typename T, typename = void>
struct HasSchema : std::false_type
{
};

/// @brief Специализация для типов со статической схемой kSchema
template <typename T>
struct HasSchema<T, std::void_t<decltype(T::kSchema)>> : std::true_type
{
};

/// @brief true, если T имеет kSchema
template <typename T>
inline constexpr bool HasSchemaV = HasSchema<std::remove_reference_t<T>>::value;

/// @brief Признак того, что тип является std::vector
/// @tparam T Проверяемый тип
template <typename T>
struct IsVector : std::false_type
{
};

/// @brief Специализация для std::vector
template <typename U, typename A>
struct IsVector<std::vector<U, A>> : std::true_type
{
};

/// @brief true, если T является std::vector
template <typename T>
inline constexpr bool IsVectorV = IsVector<std::remove_reference_t<T>>::value;

/// @brief Обходит все поля объекта согласно его статической схеме kSchema
/// @details Для каждого поля вызывает fn(значение_поля, строковое_имя). Имя берётся из
///          второго элемента пары схемы, ссылка на поле — через указатель-на-член.
/// @tparam T Тип объекта (должен иметь static kSchema)
/// @tparam Fn Тип визитора с сигнатурой fn(auto& field, const char* name)
/// @param[in,out] object Объект, поля которого обходятся
/// @param[in] fn Визитор, применяемый к каждому полю
template <typename T, typename Fn>
void ForEachField(T& object, Fn&& fn)
{
    std::apply([&](auto&&... entry) { (fn(object.*(entry.first), entry.second), ...); },
               std::remove_reference_t<T>::kSchema);
}

/// @brief Перегрузка ForEachField для константного объекта.
/// @tparam T Тип объекта (должен иметь static kSchema)
/// @tparam Fn Тип визитора с сигнатурой fn(const auto& field, const char* name)
/// @param[in] object Объект, поля которого обходятся (read-only)
/// @param[in] fn Визитор, применяемый к каждому полю
template <typename T, typename Fn>
void ForEachField(const T& object, Fn&& fn)
{
    std::apply([&](auto&&... entry) { (fn(object.*(entry.first), entry.second), ...); },
               std::remove_reference_t<T>::kSchema);
}

}  // namespace data

#endif  // GUID_b3f1d2c4_5a6e_4f8b_9c0d_1e2f3a4b5c6d
