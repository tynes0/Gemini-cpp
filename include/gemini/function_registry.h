#pragma once

#ifndef GEMINI_FUNCTION_REGISTRY_H
#define GEMINI_FUNCTION_REGISTRY_H

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <tuple>
#include <nlohmann/json.hpp>

#include "internal/function_utils.h"
#include "logger.h"
#include "types/generating_content_api_types.h"

namespace GeminiCPP
{
    class FunctionRegistry
    {
    public:
        struct RegisteredFunction
        {
            FunctionDeclaration declaration;
            std::function<nlohmann::json(const nlohmann::json&)> invoker; 
        };

        template <typename T>
        struct function_traits;

        // Lambda / Functor
        template <typename ClassType, typename ReturnT, typename... Args>
        struct function_traits<ReturnT(ClassType::*)(Args...) const>
        {
            using ArgsTuple = std::tuple<Args...>;
            using ReturnType = ReturnT;
        };
        
        template <typename ReturnT, typename... Args>
        struct function_traits<ReturnT(*)(Args...)> {
            using ArgsTuple = std::tuple<Args...>;
            using ReturnType = ReturnT;
        };

        // Functor Wrapper
        template <typename T>
        struct function_traits : public function_traits<decltype(&T::operator())> {};

        template <typename Func>
        void registerFunction(const std::string& name, Func&& func, const std::string& description, const std::vector<std::string>& argNames)
        {
            using Traits = function_traits<std::decay_t<Func>>;
            
            FunctionDeclaration decl;
            decl.name = name;
            decl.description = description;
            decl.parameters = generateSchema<typename Traits::ArgsTuple>(argNames);

            auto invoker = [func = std::forward<Func>(func), argNames](const nlohmann::json& args) -> nlohmann::json
            {
                try
                {
                    auto tupleArgs = Internal::jsonToTuple<typename Traits::ArgsTuple>(args, argNames);
                    
                    if constexpr (std::is_void_v<typename Traits::ReturnType>)
                    {
                        std::apply(func, tupleArgs);
                        return {{"result", "ok"}};
                    } else
                    {
                        return std::apply(func, tupleArgs);
                    }
                }
                catch (const std::exception& e)
                {
                    GEMINI_ERROR("Function execution failed: {}", e.what());
                    return {{"error", e.what()}};
                }
            };

            functions_[name] = {decl, invoker};
            GEMINI_INFO("Function registered: {}", name);
        }

        [[nodiscard]] std::optional<nlohmann::json> invoke(const std::string& name, const nlohmann::json& args)
        {
            if (functions_.contains(name))
            {
                return functions_[name].invoker(args);
            }
            return std::nullopt;
        }

        [[nodiscard]] Tool getTool() const
        {
            Tool t;
            for (const auto& reg : std::views::values(functions_))
            {
                t.functionDeclarations.push_back(reg.declaration);
            }
            return t;
        }

        [[nodiscard]] bool empty() const { return functions_.empty(); }

    private:
        std::map<std::string, RegisteredFunction> functions_;

        template <typename TupleType, std::size_t... Is>
        nlohmann::json generateSchemaImpl(const std::vector<std::string>& argNames, std::index_sequence<Is...>)
        {
            nlohmann::json properties = nlohmann::json::object();
            nlohmann::json required = nlohmann::json::array();

            (..., (
                properties[argNames[Is]] = Internal::JsonTypeTraits<std::tuple_element_t<Is, TupleType>>::schema(),
                required.push_back(argNames[Is])
            ));

            return {
                {"type", "object"},
                {"properties", properties},
                {"required", required}
            };
        }

        template <typename TupleType>
        nlohmann::json generateSchema(const std::vector<std::string>& argNames)
        {
            return generateSchemaImpl<TupleType>(argNames, std::make_index_sequence<std::tuple_size_v<TupleType>>{});
        }
    };
}

#endif // GEMINI_FUNCTION_REGISTRY_H