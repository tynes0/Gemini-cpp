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
    template <typename T>
    struct JsonTypeTraits;

    template <>
    struct JsonTypeTraits<std::string>
    {
        static nlohmann::json schema() { return {{"type", "string"}}; }
    };
    
    template <>
    struct JsonTypeTraits<const char*>
    {
        static nlohmann::json schema() { return {{"type", "string"}}; }
    };

    template <typename T>
    requires std::is_integral_v<T> && (!std::is_same_v<T, bool>)
    struct JsonTypeTraits<T>
    {
        static nlohmann::json schema() { return {{"type", "integer"}}; }
    };

    template <typename T>
    requires std::is_floating_point_v<T>
    struct JsonTypeTraits<T>
    {
        static nlohmann::json schema() { return {{"type", "number"}}; }
    };

    template <>
    struct JsonTypeTraits<bool>
    {
        static nlohmann::json schema() { return {{"type", "boolean"}}; }
    };

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

    template <typename T>
    T parseArg(const nlohmann::json& j, const std::string& name)
    {
        if (!j.contains(name))
        {
            if constexpr (std::is_default_constructible_v<T>)
                return T{};
            
            throw std::runtime_error("Missing argument: " + name);
        }
        return j.at(name).get<T>();
    }

    template <typename... Args, std::size_t... Is>
    std::tuple<Args...> jsonToTuple(const nlohmann::json& j, const std::vector<std::string>& argNames, std::index_sequence<Is...>) 
    {
        return std::make_tuple(parseArg<std::decay_t<Args>>(j, argNames[Is])...);
    }
}

#endif