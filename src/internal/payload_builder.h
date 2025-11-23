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
            const std::string& systemInstruction = "",
            const GenerationConfig& config = {},
            const std::vector<SafetySetting>& safetySettings = {},
            const std::vector<Tool>& tools = {}
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

            payload["generationConfig"] = config.toJson();

            if (!safetySettings.empty())
                {
                nlohmann::json safetyJson = nlohmann::json::array();
                for (const auto& setting : safetySettings) {
                    safetyJson.push_back(setting.toJson());
                }
                payload["safetySettings"] = safetyJson;
            }

            if (!tools.empty())
            {
                nlohmann::json toolsJson = nlohmann::json::array();
                for (const auto& t : tools)
                {
                    toolsJson.push_back(t.toJson());
                }
                payload["tools"] = toolsJson;
            }

            return payload;
        }
    };

}