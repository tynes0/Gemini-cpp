#include "gemini/types/batch_api_types.h"

namespace GeminiCPP
{
    GenerateContentRequest GenerateContentRequest::fromJson(const nlohmann::json& j)
    {
        GenerateContentRequest result;
        
        result.model = j.value("model", "");

        if (j.contains("contents"))
        {
            result.contents.reserve(j["contents"].size());
            
            for (const auto& content : j["contents"])
                result.contents.push_back(Content::fromJson(content));
        }

        if (j.contains("tools"))
        {
            std::vector<Tool> tools;
            tools.reserve(j["tools"].size());

            for (const auto& tool : j["tools"])
                tools.push_back(Tool::fromJson(tool));
            
            result.tools = tools;
        }

        if (j.contains("toolConfig"))
        {
            result.toolConfig = ToolConfig::fromJson(j["toolConfig"]);
        }

        if (j.contains("safetySettings"))
        {
            std::vector<SafetySetting> safetySettings;
           safetySettings.reserve(j["safetySettings"].size());
            
            for (const auto& safetySetting : j["safetySettings"])
               safetySettings.push_back(SafetySetting::fromJson(safetySetting));
            result.safetySettings = safetySettings;
        }

        if (j.contains("systemInstruction"))
        {
            result.systemInstruction = Content::fromJson(j["systemInstruction"]);
        }

        if (j.contains("cachedContent"))
        {
            result.cachedContent = j["cachedContent"].get<std::string>();
        }
        
        return result;
    }

    nlohmann::json GenerateContentRequest::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["model"] = model.str();
        
        j["contents"] = nlohmann::json::array();
        for (const auto& content : contents)
            j["contents"].push_back(content.toJson());

        if (tools.has_value())
        {
            j["tools"] = nlohmann::json::array();
            for (const auto& tool : *tools)
                j["tools"].push_back(tool.toJson());
        }

        if (toolConfig.has_value())
        {
            j["toolConfig"] = toolConfig->toJson();
        }

        if (safetySettings.has_value())
        {
            j["safetySettings"] = nlohmann::json::array();
            for (const auto& safetySetting : *safetySettings)
                j["safetySettings"].push_back(safetySetting.toJson());
        }

        if (systemInstruction.has_value())
        {
            j["systemInstruction"] = systemInstruction->toJson();
        }

        if (generationConfig.has_value())
        {
            j["generationConfig"] = generationConfig->toJson();
        }

        if (cachedContent.has_value())
        {
            j["cachedContent"] = *cachedContent;
        }
        
        return j;
    }
}
