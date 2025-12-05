#pragma once

#ifndef GEMINI_FUNCTION_UTILS_H
#define GEMINI_FUNCTION_UTILS_H

#include <string>
#include <vector>
#include <tuple>
#include <type_traits>
#include <nlohmann/json.hpp>

#include "frenum.h" 

namespace GeminiCPP::Internal
{
    /**
     * @brief Type traits for mapping C++ types to JSON Schema types.
     * @tparam T The C++ type to map.
     */
    template <typename T>
    struct JsonTypeTraits;

    /**
     * @brief Specialization for std::string. Maps to {"type": "string"}.
     */
    template <>
    struct JsonTypeTraits<std::string>
    {
        static nlohmann::json schema() { return {{"type", "string"}}; }
    };
    
    /**
     * @brief Specialization for C-strings. Maps to {"type": "string"}.
     */
    template <>
    struct JsonTypeTraits<const char*>
    {
        static nlohmann::json schema() { return {{"type", "string"}}; }
    };

    /**
     * @brief Specialization for integral types (excluding bool). Maps to {"type": "integer"}.
     */
    template <typename T>
    requires std::is_integral_v<T> && (!std::is_same_v<T, bool>)
    struct JsonTypeTraits<T>
    {
        static nlohmann::json schema() { return {{"type", "integer"}}; }
    };

    /**
     * @brief Specialization for floating point types. Maps to {"type": "number"}.
     */
    template <typename T>
    requires std::is_floating_point_v<T>
    struct JsonTypeTraits<T>
    {
        static nlohmann::json schema() { return {{"type", "number"}}; }
    };

    /**
     * @brief Specialization for boolean. Maps to {"type": "boolean"}.
     */
    template <>
    struct JsonTypeTraits<bool>
    {
        static nlohmann::json schema() { return {{"type", "boolean"}}; }
    };

    /**
     * @brief Specialization for std::vector (arrays). Maps to {"type": "array", "items": ...}.
     */
    template <typename T>
    struct JsonTypeTraits<std::vector<T>>
    {
        static nlohmann::json schema()
        {
            return {
                {"type", "array"},
                {"items", JsonTypeTraits<T>::schema()}
            };
        }
    };

    /**
     * @brief Specialization for Frenum enums. Maps to {"type": "string", "enum": [...]}.
     */
    template <typename T>
    requires frenum::is_frenum_v<T>
    struct JsonTypeTraits<T>
    {
        static nlohmann::json schema()
        {
            nlohmann::json enumValues = nlohmann::json::array();
            for (const auto& [val, name] : frenum::enum_data_registry<T>::type::values)
            {
                enumValues.push_back(name);
            }
            return {
                {"type", "string"},
                {"enum", enumValues}
            };
        }
    };

    /**
     * @brief Parses a named argument from a JSON object into type T.
     * @tparam T The target C++ type.
     * @param j The JSON object containing arguments.
     * @param name The key name of the argument.
     * @return The parsed value.
     * @throws std::runtime_error if the argument is missing (and T is not default constructible) or invalid.
     */
    template <typename T>
    T parseArg(const nlohmann::json& j, const std::string& name)
    {
        if (!j.contains(name))
        {
            if constexpr
            (std::is_default_constructible_v<T>)
                return T{};
            
            throw std::runtime_error("Missing argument: " + name);
        }
        
        if constexpr (frenum::is_frenum_v<T>)
        {
            auto val = frenum::cast<T>(j.at(name).get<std::string>());
            if(val)
                return *val;
            
            throw std::runtime_error("Invalid enum value for: " + name);
        } 
        else
        {
            return j.at(name).get<T>();
        }
    }

    // Helper implementation for converting JSON to Tuple
    template <typename TupleType, std::size_t... Is>
    TupleType jsonToTupleImpl(const nlohmann::json& j, const std::vector<std::string>& argNames, std::index_sequence<Is...>) 
    {
        return std::make_tuple(parseArg<std::tuple_element_t<Is, TupleType>>(j, argNames[Is])...);
    }

    /**
     * @brief Converts a JSON object of arguments into a std::tuple matching function signature.
     * @tparam TupleType The target std::tuple type representing function arguments.
     * @param j The JSON object containing arguments.
     * @param argNames List of argument names corresponding to the tuple elements.
     * @return A tuple populated with values from the JSON.
     */
    template <typename TupleType>
    TupleType jsonToTuple(const nlohmann::json& j, const std::vector<std::string>& argNames) 
    {
        return jsonToTupleImpl<TupleType>(j, argNames, std::make_index_sequence<std::tuple_size_v<TupleType>>{});
    }
}

#endif