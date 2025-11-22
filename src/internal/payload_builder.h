#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include "gemini/types.h"

namespace GeminiCPP::Internal
{
    class PayloadBuilder
    {
    public:
        static nlohmann::json build(
            const std::vector<Content>& contents,
            const std::string& systemInstruction = ""
            // TODO: 'SafetySettings', 'GenerationConfig' 
        ) {
            nlohmann::json contentsJson = nlohmann::json::array();
            for (const auto& msg : contents)
                contentsJson.push_back(msg.toJson());

            nlohmann::json payload = {
                {"contents", contentsJson}
            };

            if (!systemInstruction.empty())
            {
                payload["system_instruction"] = {
                    {"parts", {{ {"text", systemInstruction} }}}
                };
            }

            return payload;
        }
    };

}