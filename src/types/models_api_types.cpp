#include "gemini/types/models_api_types.h"

namespace GeminiCPP
{
    PredictRequestBody PredictRequestBody::fromJson(const nlohmann::json& j)
    {
        PredictRequestBody result{};

        if (j.contains("instances")) result.instances = j["instances"].get<std::vector<nlohmann::json>>();
        if (j.contains("parameters")) result.parameters = j["parameters"];

        return result;
    }

    nlohmann::json PredictRequestBody::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["instances"] = instances;
        if (parameters.has_value())
            j["parameters"] = parameters.value();

        return j;
    }

    PredictResponseBody PredictResponseBody::fromJson(const nlohmann::json& j)
    {
        PredictResponseBody result{};

        if (j.contains("predictions"))
            result.predictions = j["predictions"];
        
        return result;
    }

    PredictLongRunningRequestBody PredictLongRunningRequestBody::fromJson(const nlohmann::json& j)
    {
        PredictLongRunningRequestBody result{};

        if (j.contains("instances")) result.instances = j["instances"].get<std::vector<nlohmann::json>>();
        if (j.contains("parameters")) result.parameters = j["parameters"];

        return result;
    }

    nlohmann::json PredictLongRunningRequestBody::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["instances"] = instances;
        if (parameters.has_value())
            j["parameters"] = parameters.value();

        return j;
    }

    nlohmann::json PredictResponseBody::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["predictions"] = predictions;
        
        return j;
    }

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
        nlohmann::json j = nlohmann::json::object();

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

    ModelListResponseBody ModelListResponseBody::fromJson(const nlohmann::json& j)
    {
        ModelListResponseBody result{};

        if (j.contains("models"))
        {
            result.models.reserve(j["models"].size());
            for (const auto& model : j["models"])
                result.models.push_back(ModelInfo::fromJson(model));
        }

        result.nextPageToken = j.value("nextPageToken", "");

        return result;
    }

    nlohmann::json ModelListResponseBody::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["models"] = nlohmann::json::array();
        for (const auto& model : models)
            j["models"].push_back(model.toJson());

        j["nextPageToken"] = nextPageToken;
        
        return j;
    }

    ModelListQueryParameters ModelListQueryParameters::fromJson(const nlohmann::json& j)
    {
        ModelListQueryParameters result{};

        result.pageSize = j.value("pageSize", 0);
        result.pageToken = j.value("pageToken", "");

        return result;
    }

    nlohmann::json ModelListQueryParameters::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["pageSize"] = pageSize;
        j["pageToken"] = pageToken;

        return j;
    }
}
