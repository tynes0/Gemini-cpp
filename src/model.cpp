#include "gemini/model.h"

namespace GeminiCPP
{
    std::string_view ModelHelper::stringRepresentation(Model model)
    {
        switch (model)
        {
        case Model::GEMINI_2_0_FLASH:               return "gemini-2.0-flash";
        case Model::GEMINI_2_0_FLASH_LITE:          return "gemini-2.0-flash-lite";
        case Model::GEMINI_2_5_FLASH:               return "gemini-2.5-flash";
        case Model::GEMINI_2_5_FLASH_IMAGE:         return "gemini-2.5-flash-image";
        case Model::GEMINI_2_5_FLASH_TTS:           return "gemini-2.5-flash-preview-tts";
        case Model::GEMINI_2_5_FLASH_LITE:          return "gemini-2.5-flash-lite";
        case Model::GEMINI_2_5_PRO:                 return "gemini-2.5-pro";
        case Model::GEMINI_2_5_PRO_TTS:             return "gemini-2.5-pro-preview-tts";
        case Model::GEMINI_3_0_PRO_PREVIEW:         return "gemini-3-pro-preview";
        case Model::GEMINI_3_0_PRO_IMAGE_PREVIEW:   return "gemini-3-pro-image-preview";
        case Model::GEMINI_FLASH_LATEST:            return "gemini-flash-latest";
        case Model::GEMINI_PRO_LATEST:              return "gemini-pro-latest";
        }
        return "gemini-2.5-flash";
    }

    Model ModelHelper::fromStringRepresentation(const std::string& str)
    {
        if (str == "gemini-2.0-flash") return Model::GEMINI_2_0_FLASH;
        if (str == "gemini-2.0-flash-lite") return Model::GEMINI_2_0_FLASH_LITE;
        if (str == "gemini-2.5-flash") return Model::GEMINI_2_5_FLASH;
        if (str == "gemini-2.5-flash-image") return Model::GEMINI_2_5_FLASH_IMAGE;
        if (str == "gemini-2.5-flash-preview-tts") return Model::GEMINI_2_5_FLASH_TTS;
        if (str == "gemini-2.5-flash-lite") return Model::GEMINI_2_5_FLASH_LITE;
        if (str == "gemini-2.5-pro") return Model::GEMINI_2_5_PRO;
        if (str == "gemini-2.5-pro-preview-tts") return Model::GEMINI_2_5_PRO_TTS;
        if (str == "gemini-3-pro-preview") return Model::GEMINI_3_0_PRO_PREVIEW;
        if (str == "gemini-3-pro-image-preview") return Model::GEMINI_3_0_PRO_IMAGE_PREVIEW;
        if (str == "gemini-flash-latest") return Model::GEMINI_FLASH_LATEST;
        if (str == "gemini-pro-latest") return Model::GEMINI_PRO_LATEST;

        return Model::GEMINI_2_5_FLASH;
    }

    bool ModelInfo::supports(GenerationMethod method) const
    {
        return (supportedGenerationMethods & method) != 0;
    }
    
    ModelInfo ModelInfo::fromJson(const nlohmann::json& j)
    {
        ModelInfo info;
        if(j.contains("name"))
            info.name = j.value("name", "");
        if(j.contains("version"))
            info.version = j.value("version", "");
        if(j.contains("displayName"))
            info.displayName = j.value("displayName", "");
        if(j.contains("description"))
            info.description = j.value("description", "");
            
        info.inputTokenLimit = j.value("inputTokenLimit", 0);
        info.outputTokenLimit = j.value("outputTokenLimit", 0);
        info.temperature = j.value("temperature", 0.0);
        info.topP = j.value("topP", 0.0);
        info.topK = j.value("topK", 0);

        info.supportedGenerationMethods = GM_NONE;
        if(j.contains("supportedGenerationMethods"))
        {
            for(const auto& methodJson : j["supportedGenerationMethods"])
                info.supportedGenerationMethods |= GenerationMethodHelper::fromString(methodJson.get<std::string>());
        }
        return info;
    }

    std::string ModelInfo::toString() const
    {
        return "Model: " + displayName + " (" + name + ")\n" +
               "Desc: " + description + "\n" +
               "Tokens: In=" + std::to_string(inputTokenLimit) + ", Out=" + std::to_string(outputTokenLimit) + "\n" +
               "Methods: " + GenerationMethodHelper::bitmaskToString(supportedGenerationMethods);
    }
}
