#include "gemini/types/models_api_types.h"

namespace GeminiCPP
{
    bool ModelInfo::supports(GenerationMethod method) const
    {
        return (supportedGenerationMethods & method) != 0;
    }
    
    ModelInfo ModelInfo::fromJson(const nlohmann::json& j)
    {
        ModelInfo info{};
        info.name = j.value("name", "");
        info.baseModelId = j.value("baseModelId", "");
        info.version = j.value("version", "");
        info.displayName = j.value("displayName", "");
        info.description = j.value("description", "");
        info.inputTokenLimit = j.value("inputTokenLimit", 0);
        info.outputTokenLimit = j.value("outputTokenLimit", 0);
        info.thinking = j.value("thinking", false);
        info.temperature = j.value("temperature", 0.0f);
        info.maxTemperature = j.value("maxTemperature", 0.0f);
        info.topP = j.value("topP", 0.0f);
        info.topK = j.value("topK", 0);

        info.supportedGenerationMethods = GM_NONE;
        if(j.contains("supportedGenerationMethods"))
        {
            for(const auto& methodJson : j["supportedGenerationMethods"])
                info.supportedGenerationMethods |= GenerationMethodHelper::fromString(methodJson.get<std::string>());
        }
        return info;
    }

    nlohmann::json ModelInfo::toJson() const
    {
        nlohmann::json j;

        j["name"] = name.str();
        j["baseModelId"] = baseModelId.str();
        j["version"] = version;
        j["displayName"] = displayName;
        j["description"] = description;
        j["inputTokenLimit"] = inputTokenLimit;
        j["outputTokenLimit"] = outputTokenLimit;
        j["thinking"] = thinking;
        j["temperature"] = temperature;
        j["maxTemperature"] = maxTemperature;
        j["topP"] = topP;
        j["topK"] = topK;
        j["supportedGenerationMethods"] = GenerationMethodHelper::bitmaskToStringArray(supportedGenerationMethods);
        
        return j;
    }
}