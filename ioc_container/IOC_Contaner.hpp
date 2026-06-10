/// @file IOC_Contaner.hpp
/// @brief Контейнер внедрения зависимостей IoC.
/// @author Artemenko Anton

#ifndef GUID_4d7c2a91_f3e8_4c4a_9a12_6b3f8d0e71c5
#define GUID_4d7c2a91_f3e8_4c4a_9a12_6b3f8d0e71c5

#include <functional>
#include <map>
#include <memory>
#include <utility>

namespace ioc::container
{
/// @brief Контейнер внедрения зависимостей.
/// @details
/// Контракт по потокобезопасности (важно):
///   * Регистрация (RegisterInstance/RegisterFactory/RegisterFunctor) НЕ потокобезопасна.
///     Изменение factories_ и счётчика nextTypeId_ не синхронизировано.
///   * Разрешение (GetObject<>) безопасно для чтения ТОЛЬКО при условии, что вся регистрация
///     уже завершена ДО старта дополнительных потоков. После этого factories_ выступает
///     как фактически иммутабельный словарь, чтение из std::map без модификации — безопасно.
///   * Параллельная регистрация + резолв из разных потоков приведёт к гонке и/или порче map.
///
/// Типичный сценарий: создать контейнер на стеке в композиционном корне, выполнить все
/// RegisterXxx, затем все GetObject, после чего контейнер можно уничтожить — в фабриках уже
/// захвачены все необходимые shared_ptr. Рабочие потоки запускать только ПОСЛЕ этой фазы.
class IOCContainer
{
    inline static int nextTypeId_ = 115094801;  ///< Счётчик для генерации идентификаторов типов;
                                                ///< не синхронизирован, см. контракт класса.

    /// @brief Возвращает уникальный идентификатор для типа `T`.
    /// @tparam T Тип, для которого создаётся идентификатор.
    /// @return Уникальный идентификатор типа.
    template <typename T>
    static int GetTypeID()
    {
        static int typeId = nextTypeId_++;
        return typeId;
    }

   public:
    /// @brief Базовый интерфейс для всех фабрик — нетипизированный узел в factories_.
    class FactoryRoot
    {
       public:
        /// @brief Виртуальный деструктор базовой фабрики.
        virtual ~FactoryRoot() = default;
    };

    std::map<int, std::shared_ptr<FactoryRoot>>
        factories_;  ///< Хранилище зарегистрированных фабрик по идентификатору типа.

    /// @brief Фабрика, возвращающая объект типа `T`.
    /// @tparam T Тип создаваемого объекта.
    template <typename T>
    class CFactory : public FactoryRoot
    {
        std::function<std::shared_ptr<T>()> functor_;  ///< Функтор создания объекта.

       public:
        /// @brief Виртуальный деструктор фабрики.
        ~CFactory() override = default;

        /// @brief Создаёт фабрику с заданным функтором.
        /// @param[in] functor Функтор создания объекта.
        explicit CFactory(std::function<std::shared_ptr<T>()> functor) : functor_(std::move(functor))
        {
        }

        /// @brief Возвращает новый экземпляр объекта.
        /// @return Созданный объект.
        std::shared_ptr<T> GetObject()
        {
            return functor_();
        }
    };

    /// @brief Получает зарегистрированный объект типа `T`.
    /// @tparam T Тип объекта.
    /// @return Зарегистрированный объект.
    template <typename T>
    std::shared_ptr<T> GetObject()
    {
        auto typeId = GetTypeID<T>();
        auto factoryBase = factories_[typeId];
        auto factory = std::static_pointer_cast<CFactory<T>>(factoryBase);
        return factory->GetObject();
    }

    /// @brief Регистрирует функтор создания объекта.
    /// @tparam TInterface Тип интерфейса.
    /// @tparam TS Типы зависимостей.
    /// @param[in] functor Функтор создания объекта.
    template <typename TInterface, typename... TS>
    void RegisterFunctor(std::function<std::shared_ptr<TInterface>(std::shared_ptr<TS>... ts)> functor)
    {
        factories_[GetTypeID<TInterface>()] =
            std::make_shared<CFactory<TInterface>>([=] { return functor(GetObject<TS>()...); });
    }

    /// @brief Регистрирует уже созданный экземпляр.
    /// @tparam TInterface Тип интерфейса.
    /// @param[in] t Зарегистрированный объект.
    template <typename TInterface>
    void RegisterInstance(std::shared_ptr<TInterface> t)
    {
        factories_[GetTypeID<TInterface>()] = std::make_shared<CFactory<TInterface>>([=] { return t; });
    }

    /// @brief Регистрирует функтор по указателю на функцию.
    /// @tparam TInterface Тип интерфейса.
    /// @tparam TS Типы зависимостей.
    /// @param[in] functor Указатель на функцию создания объекта.
    template <typename TInterface, typename... TS>
    void RegisterFunctor(std::shared_ptr<TInterface> (*functor)(std::shared_ptr<TS>... ts))
    {
        RegisterFunctor(std::function<std::shared_ptr<TInterface>(std::shared_ptr<TS>... ts)>(functor));
    }

    /// @brief Регистрирует фабрику, создающую новый экземпляр при каждом запросе.
    /// @tparam TInterface Тип интерфейса.
    /// @tparam TConcrete Тип конкретной реализации.
    /// @tparam TArguments Типы зависимостей конструктора.
    template <typename TInterface, typename TConcrete, typename... TArguments>
    void RegisterFactory()
    {
        RegisterFunctor(std::function<std::shared_ptr<TInterface>(std::shared_ptr<TArguments>... ts)>(
            [](std::shared_ptr<TArguments>... arguments) -> std::shared_ptr<TInterface> {
                return std::make_shared<TConcrete>(std::forward<std::shared_ptr<TArguments>>(arguments)...);
            }));
    }

    /// @brief Регистрирует единственный экземпляр конкретной реализации.
    /// @tparam TInterface Тип интерфейса.
    /// @tparam TConcrete Тип конкретной реализации.
    /// @tparam TArguments Типы зависимостей конструктора.
    template <typename TInterface, typename TConcrete, typename... TArguments>
    void RegisterInstance()
    {
        RegisterInstance<TInterface>(std::make_shared<TConcrete>(GetObject<TArguments>()...));
    }
};
}  // namespace ioc::container

#endif  // GUID_4d7c2a91_f3e8_4c4a_9a12_6b3f8d0e71c5