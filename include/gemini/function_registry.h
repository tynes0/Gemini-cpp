#pragma once

#ifndef GEMINI_FUNCTION_REGISTRY_H
#define GEMINI_FUNCTION_REGISTRY_H

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <tuple>
#include <nlohmann/json.hpp>

#include "types.h"
#include "internal/function_utils.h"
#include "logger.h"

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
                    // JSON -> Tuple
                    auto tupleArgs = Internal::jsonToTuple<typename Traits::ArgsTuple>(args, argNames, std::make_index_sequence<std::tuple_size_v<typename Traits::ArgsTuple>>{});

                    if constexpr (std::is_void_v<typename Traits::ReturnType>)
                    {
                        std::apply(func, tupleArgs);
                        return {{"result", "ok"}};
                    }
                    else
                    {
                        auto result = std::apply(func, tupleArgs);
                        return result;
                    }
                }
                catch (const std::exception& e)
                {
                    GEMINI_ERROR("Function execution failed: {}", e.what());
                    return {{"error", e.what()}};
                }
            };

            functions_[name] = {decl, invoker};
        }

        [[nodiscard]] std::optional<nlohmann::json> invoke(const std::string& name, const nlohmann::json& args) {
            if (functions_.contains(name))
            {
                return functions_[name].invoker(args);
            }
            return std::nullopt;
        }

        [[nodiscard]] std::vector<FunctionDeclaration> getDeclarations() const
        {
            std::vector<FunctionDeclaration> decls;
            for (const auto& reg : std::views::values(functions_))
            {
                decls.push_back(reg.declaration);
            }
            return decls;
        }
        
        [[nodiscard]] Tool getTool() const
        {
            Tool t;
            t.functionDeclarations = getDeclarations();
            return t;
        }

    private:
        std::map<std::string, RegisteredFunction> functions_;

        template <typename T> struct function_traits;

        template <typename ClassType, typename ReturnT, typename... Args>
        struct function_traits<ReturnT(ClassType::*)(Args...) const>
        {
            using ArgsTuple = std::tuple<Args...>;
            using ReturnType = ReturnT;
        };
        
        template <typename ReturnT, typename... Args>
        struct function_traits<ReturnT(*)(Args...)>
        {
            using ArgsTuple = std::tuple<Args...>;
            using ReturnType = ReturnT;
        };

        template <typename T>
        struct function_traits : public function_traits<decltype(&T::operator())> {};

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