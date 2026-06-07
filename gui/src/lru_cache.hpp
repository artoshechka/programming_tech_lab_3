/// @file lru_cache.hpp
/// @brief Generic LRU-кэш (header-only)
/// @author Artemenko Anton

#ifndef GUID_8a4c2d10_2b6e_4c1f_b18a_1f3e9d4c20a1
#define GUID_8a4c2d10_2b6e_4c1f_b18a_1f3e9d4c20a1

#include <cstddef>
#include <list>
#include <unordered_map>
#include <utility>

namespace gui
{

/// @brief Простой LRU-кэш ключ → значение с фиксированной ёмкостью.
/// @tparam K Тип ключа (требует hash и equality).
/// @tparam V Тип значения.
/// @details
/// Эвикт — последний использованный элемент. Сложность Find/Put — амортизированно O(1).
/// Не потокобезопасен: предполагается использование из одного потока (GUI-thread).
template <typename K, typename V>
class LruCache
{
   public:
    /// @brief Создаёт кэш с заданной ёмкостью.
    /// @param[in] capacity Максимальное число элементов; при превышении вытесняется LRU.
    explicit LruCache(std::size_t capacity) : capacity_(capacity == 0 ? 1 : capacity) {}

    /// @brief Возвращает указатель на значение по ключу либо nullptr.
    /// @param[in] key Ключ для поиска.
    /// @return Указатель на сохранённое значение; nullptr при промахе.
    /// @details Найденный элемент перемещается в начало (становится MRU).
    V* Find(const K& key)
    {
        auto it = index_.find(key);
        if (it == index_.end()) return nullptr;
        order_.splice(order_.begin(), order_, it->second);
        return &it->second->second;
    }

    /// @brief Помещает значение в кэш; вытесняет LRU при переполнении.
    /// @param[in] key Ключ.
    /// @param[in] value Значение (передаётся по value, поддерживает move).
    void Put(const K& key, V value)
    {
        auto it = index_.find(key);
        if (it != index_.end())
        {
            it->second->second = std::move(value);
            order_.splice(order_.begin(), order_, it->second);
            return;
        }
        if (order_.size() >= capacity_)
        {
            const auto& evict = order_.back();
            index_.erase(evict.first);
            order_.pop_back();
        }
        order_.emplace_front(key, std::move(value));
        index_[key] = order_.begin();
    }

    /// @brief Очищает кэш.
    void Clear()
    {
        index_.clear();
        order_.clear();
    }

    /// @brief Текущий размер кэша.
    std::size_t Size() const { return order_.size(); }

   private:
    using Entry = std::pair<K, V>;
    using List = std::list<Entry>;

    std::size_t capacity_;                                       ///< Максимальное число элементов.
    List order_;                                                 ///< MRU спереди, LRU сзади.
    std::unordered_map<K, typename List::iterator> index_;       ///< Индекс ключ → итератор списка.
};

}  // namespace gui

#endif  // GUID_8a4c2d10_2b6e_4c1f_b18a_1f3e9d4c20a1
