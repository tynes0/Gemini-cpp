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

    template <typename TupleType, std::size_t... Is>
    TupleType jsonToTupleImpl(const nlohmann::json& j, const std::vector<std::string>& argNames, std::index_sequence<Is...>) 
    {
        return std::make_tuple(parseArg<std::tuple_element_t<Is, TupleType>>(j, argNames[Is])...);
    }

    template <typename TupleType>
    TupleType jsonToTuple(const nlohmann::json& j, const std::vector<std::string>& argNames) 
    {
        return jsonToTupleImpl<TupleType>(j, argNames, std::make_index_sequence<std::tuple_size_v<TupleType>>{});
    }
}

#endif