#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#include "gemini/types/generating_content_api_types.h"
#include "gemini/types/embeddings_api_types.h"

namespace GeminiCPP::Internal
{
    class PayloadBuilder
    {
    public:
        static nlohmann::json build(
            const std::vector<Content>& contents,
            const std::string& systemInstruction = "",
            const std::string& cachedContent = "",
            const GenerationConfig& config = {},
            const std::vector<SafetySetting>& safetySettings = {},
            const std::vector<Tool>& tools = {},
            const std::optional<ToolConfig>& toolConfig = {}
        ) {
            nlohmann::json contentsJson = nlohmann::json::array();
            for (const auto& msg : contents)
                contentsJson.push_back(msg.toJson());

            nlohmann::json payload = {
                {"contents", contentsJson}
            };

            if (!cachedContent.empty()) {
                payload["cachedContent"] = cachedContent;
            }

            if (!systemInstruction.empty())
            {
                payload["system_instruction"] = {
                    {"parts", {{ {"text", systemInstruction} }}}
                };
            }

            auto configJson = config.toJson();
            if (!configJson.empty())
            {
                payload["generationConfig"] = configJson;
            }

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

            if (toolConfig.has_value())
            {
                payload["toolConfig"] = toolConfig->toJson();
            }

            return payload;
        }

        static nlohmann::json buildEmbedContent(
            const Content& content,
            const std::string& modelStr,
            const EmbedRequestBody& config
        )
        {
            nlohmann::json payload = config.toJson();
            payload["content"] = content.toJson();
            payload["model"] = modelStr;
            return payload;
        }

        static nlohmann::json buildBatchEmbedContent(
            const std::vector<std::string>& texts,
            const std::string& modelStr,
            const EmbedRequestBody& config
        )
        {
            nlohmann::json requests = nlohmann::json::array();
            
            for(const auto& txt : texts)
            {
                nlohmann::json req = config.toJson();
                req["content"] = Content::User().text(txt).toJson();
                req["model"] = modelStr;
                requests.push_back(req);
            }
            return { {"requests", requests} };
        }
    };

}