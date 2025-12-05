#pragma once

#ifndef GEMINI_TYPES_H
#define GEMINI_TYPES_H

#include <nlohmann/json.hpp>

#include <type_traits>
#include <utility>

namespace GeminiCPP
{
    /**
     * @brief Base interface for JSON serialization.
     * * Provides a virtual destructor and the pure virtual `toJson` method.
     */
    struct IJsonSerializableBase
    {
        IJsonSerializableBase() = default;
        IJsonSerializableBase(const IJsonSerializableBase&) = default;
        IJsonSerializableBase& operator=(const IJsonSerializableBase&) = default;
        IJsonSerializableBase(IJsonSerializableBase&&) = default;
        IJsonSerializableBase& operator=(IJsonSerializableBase&&) = default;

        virtual ~IJsonSerializableBase() = default;

        /**
         * @brief Serializes the object to a JSON object.
         * @return nlohmann::json representation of the object.
         */
        [[nodiscard]] virtual nlohmann::json toJson() const = 0;
    };

    // Meta-programming helper to check for 'fromJson' existence
    template <typename T, typename = void>
    struct Has_fromJson : std::false_type {};

    template <typename T>
    struct Has_fromJson<T, std::void_t<decltype(T::fromJson(std::declval<const nlohmann::json&>()))>> : std::true_type {};

    /**
     * @brief CRTP (Curiously Recurring Template Pattern) base for JSON serializable types.
     * * Enforces the implementation of a static `fromJson` method in derived classes at compile time.
     * @tparam T The derived class type.
     */
    template<typename T>
    struct IJsonSerializable : IJsonSerializableBase
    {
    private:
        friend T;
        
        IJsonSerializable() = default;
        IJsonSerializable(const IJsonSerializable&) = default;
        IJsonSerializable& operator=(const IJsonSerializable&) = default;
        IJsonSerializable(IJsonSerializable&&) = default;
        IJsonSerializable& operator=(IJsonSerializable&&) = default;

    public:
        // NOLINTNEXTLINE(portability-template-virtual-member-function)
        ~IJsonSerializable() override
        {
            static_assert(Has_fromJson<T>::value,
                "CRTP ERROR: Derived class must implement 'static T fromJson(const nlohmann::json&)'.");
        }
    };
    
    // Concept helpers to restrict templates to Serializable types
    template <typename T, typename = void>
    struct IsJsonSerializable : std::false_type {};

    template <typename T>
    struct IsJsonSerializable<T, std::enable_if_t<std::is_base_of_v<IJsonSerializable<T>, T>>> : std::true_type {};

    /**
     * @brief Concept ensuring T inherits from IJsonSerializable<T>.
     */
    template <typename T>
    concept JsonSerializable = IsJsonSerializable<T>::value;
    
} // namespace GeminiCPP

#endif // GEMINI_TYPES_H