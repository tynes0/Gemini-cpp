#include "gemini/types/generating_content_api_types.h"

#include "gemini/utils.h"
#include "gemini/base.h"

namespace GeminiCPP
{
    SafetySetting SafetySetting::fromJson(const nlohmann::json& j)
    {
        SafetySetting result;
        result.category = frenum::cast<HarmCategory>(j.value("category", "")).value_or(HarmCategory::HARM_CATEGORY_UNSPECIFIED);
        result.threshold = frenum::cast<HarmBlockThreshold>(j.value("threshold", "")).value_or(HarmBlockThreshold::HARM_BLOCK_THRESHOLD_UNSPECIFIED);
        return result;
    }

    nlohmann::json SafetySetting::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["category"] = frenum::to_string(category);
        j["threshold"] = frenum::to_string(threshold);

        return j;
    }

    SafetyRating SafetyRating::fromJson(const nlohmann::json& j)
    {
        SafetyRating result;

        result.category = frenum::cast<HarmCategory>(j.value("category", "")).value_or(HarmCategory::HARM_CATEGORY_UNSPECIFIED);
        result.probability = frenum::cast<HarmProbability>(j.value("probability", "")).value_or(HarmProbability::HARM_PROBABILITY_UNSPECIFIED);
        result.blocked = j.value("blocked", false);

        return result;
    }
    
    nlohmann::json SafetyRating::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        j["category"] = frenum::to_string(category);
        j["probability"] = frenum::to_string(probability);
        j["blocked"] = blocked;
        return j;
    }
    
    ModalityTokenCount ModalityTokenCount::fromJson(const nlohmann::json& j)
    {
        ModalityTokenCount result{};
        
        result.modality = frenum::cast<Modality>(j.value("modality", "")).value_or(Modality::MODALITY_UNSPECIFIED);
        result.tokenCount = j["tokenCount"].value("tokenCount", 0);

        return result;
    }
    
    nlohmann::json ModalityTokenCount::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        j["modality"] = frenum::to_string(modality);
        j["tokenCount"] = tokenCount;
        return j;
    }

    ImageConfig ImageConfig::fromJson(const nlohmann::json& j)
    {
        ImageConfig result;
        
        if (j.contains("aspectRatio"))
            result.aspectRatio = j["aspectRatio"].get<std::string>();
        
        if (j.contains("imageSize"))
            result.imageSize = j["imageSize"].get<std::string>();

        return result;
    }

    nlohmann::json ImageConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        
        if (aspectRatio.has_value())
            j["aspectRatio"] = aspectRatio->str();

        if (imageSize.has_value())
            j["imageSize"] = imageSize->str();

        return j;
    }

    ThinkingConfig ThinkingConfig::fromJson(const nlohmann::json& j)
    {
        ThinkingConfig result{};

        result.includeThoughts = j.value("includeThoughts", false);
        result.thinkingBudget = j.value("thinkingBudget", 0);

        if (j.contains("thinkingLevel"))
            result.thinkingLevel = frenum::cast<ThinkingLevel>(j["thinkingLevel"].get<std::string>());
        
        return result;
    }

    nlohmann::json ThinkingConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        
        j["includeThoughts"] = includeThoughts;
        j["thinkingBudget"] = thinkingBudget;

        if (thinkingLevel.has_value())
            j["thinkingLevel"] = frenum::to_string(thinkingLevel.value());

        return j;
    }

    PrebuiltVoiceConfig PrebuiltVoiceConfig::fromJson(const nlohmann::json& j)
    {
        PrebuiltVoiceConfig result;
        result.voiceName = j.value("voiceName", "");
        return result;
    }

    nlohmann::json PrebuiltVoiceConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        j["voiceName"] = voiceName;
        return j;
    }

    VoiceConfig VoiceConfig::fromJson(const nlohmann::json& j)
    {
        VoiceConfig result;
        
        if (j.contains("prebuiltVoiceConfig"))
        {
            result.data = PrebuiltVoiceConfig::fromJson(j["prebuiltVoiceConfig"]);
        }

        return result;
    }

    nlohmann::json VoiceConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        
        std::visit([&j]<typename T>(const T& arg)
        {
            if constexpr (std::is_same_v<T, PrebuiltVoiceConfig>)
                j["prebuiltVoiceConfig"] = arg.toJson();
        }, data);

        return j;
    }

    SpeakerVoiceConfig SpeakerVoiceConfig::fromJson(const nlohmann::json& j)
    {
        SpeakerVoiceConfig result;

        result.speaker = j.value("speaker", "");
        result.voiceConfig = VoiceConfig::fromJson(j.value("voiceConfig", nlohmann::json::object()));
        
        return result;
    }

    nlohmann::json SpeakerVoiceConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["speaker"] = speaker;
        j["voiceConfig"] = voiceConfig.toJson();
        
        return j;
    }

    MultiSpeakerVoiceConfig MultiSpeakerVoiceConfig::fromJson(const nlohmann::json& j)
    {
        MultiSpeakerVoiceConfig result;

        if (j.contains("speakerVoiceConfigs"))
        {
            result.speakerVoiceConfigs.reserve(j["speakerVoiceConfigs"].size());
            for (const auto& speakerVoiceConfig : j["speakerVoiceConfigs"])
                result.speakerVoiceConfigs.push_back(SpeakerVoiceConfig::fromJson(speakerVoiceConfig));
        }

        return result;
    }

    nlohmann::json MultiSpeakerVoiceConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["speakerVoiceConfigs"] = nlohmann::json::array();

        for (const auto& speakerVoiceConfig : speakerVoiceConfigs)
            j["speakerVoiceConfigs"].push_back(speakerVoiceConfig.toJson());
        
        return j;
    }

    SpeechConfig SpeechConfig::fromJson(const nlohmann::json& j)
    {
        SpeechConfig result;

        result.voiceConfig = VoiceConfig::fromJson(j.value("voiceConfig", nlohmann::json::object()));
        
        if (j.contains("multiSpeakerVoiceConfig"))
            result.multiSpeakerVoiceConfig = MultiSpeakerVoiceConfig::fromJson(j["multiSpeakerVoiceConfig"]);

        if (j.contains("languageCode"))
        {
            result.languageCode.emplace(j["languageCode"].get<std::string>());
        }
         
        return result;
    }

    nlohmann::json SpeechConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["voiceConfig"] = voiceConfig.toJson();

        if (multiSpeakerVoiceConfig.has_value())
            j["multiSpeakerVoiceConfig"] = multiSpeakerVoiceConfig->toJson();

        if (languageCode.has_value())
            j["languageCode"] = languageCode->str();
        
        return j;
    }

    GenerationConfig GenerationConfig::fromJson(const nlohmann::json& j)
    {
        GenerationConfig result{};

        if (j.contains("stopSequences")) result.stopSequences = j["stopSequences"].get<std::vector<std::string>>();
        if (j.contains("responseMimeType")) result.responseMimeType = j["responseMimeType"].get<std::string>();
        if (j.contains("_responseJsonSchema")) result.responseJsonSchema = j["responseJsonSchema"]; // Am I sure? 
        if (j.contains("candidateCount")) result.candidateCount = j["candidateCount"].get<int>();
        if (j.contains("maxOutputTokens")) result.maxOutputTokens = j["maxOutputTokens"].get<int>();
        if (j.contains("temperature")) result.temperature = j["temperature"].get<float>();
        if (j.contains("topP")) result.topP = j["topP"].get<float>();
        if (j.contains("topK")) result.topK = j["topK"].get<int>();
        if (j.contains("seed")) result.seed = j["seed"].get<int64_t>();
        if (j.contains("presencePenalty")) result.presencePenalty = j["presencePenalty"].get<float>();
        if (j.contains("frequencyPenalty")) result.frequencyPenalty = j["frequencyPenalty"].get<float>();
        if (j.contains("responseLogprobs")) result.responseLogprobs = j["responseLogprobs"].get<bool>();
        if (j.contains("logprobs")) result.logprobs = j["logprobs"].get<int>();
        if (j.contains("enableEnhancedCivicAnswers")) result.enableEnhancedCivicAnswers = j["enableEnhancedCivicAnswers"].get<bool>();
        if (j.contains("speechConfig")) result.speechConfig = SpeechConfig::fromJson(j["speechConfig"]);
        if (j.contains("thinkingConfig")) result.thinkingConfig = ThinkingConfig::fromJson(j["thinkingConfig"]);
        if (j.contains("imageConfig")) result.imageConfig = ImageConfig::fromJson(j["imageConfig"]);
        if (j.contains("mediaResolution")) result.mediaResolution = frenum::cast<MediaResolution>(j["mediaResolution"].get<std::string>());
        
        if (j.contains("responseModalities"))
        {
            std::vector<Modality> modalities;
            modalities.reserve(j["responseModalities"].size());
            for (const auto& modality : j["responseModalities"])
                modalities.push_back(frenum::cast<Modality>(modality.get<std::string>()).value_or(Modality::MODALITY_UNSPECIFIED));
            result.responseModalities = modalities;
        }
        return result;
    }

    nlohmann::json GenerationConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
            
        if (stopSequences.has_value()) j["stopSequences"] = stopSequences.value();
        if (responseMimeType.has_value()) j["responseMimeType"] = responseMimeType.value();
        if (responseJsonSchema.has_value()) j["responseSchema"] = responseJsonSchema.value();
        if (candidateCount.has_value()) j["candidateCount"] = candidateCount.value();
        if (maxOutputTokens.has_value()) j["maxOutputTokens"] = maxOutputTokens.value();
        if (temperature.has_value()) j["temperature"] = temperature.value();
        if (topP.has_value()) j["topP"] = topP.value();
        if (topK.has_value()) j["topK"] = topK.value();
        if (seed.has_value()) j["seed"] = seed.value();
        if (presencePenalty.has_value()) j["presencePenalty"] = presencePenalty.value();
        if (frequencyPenalty.has_value()) j["frequencyPenalty"] = frequencyPenalty.value();
        if (responseLogprobs.has_value()) j["responseLogprobs"] = responseLogprobs.value();
        if (logprobs.has_value()) j["logprobs"] = logprobs.value();
        if (enableEnhancedCivicAnswers.has_value()) j["enableEnhancedCivicAnswers"] = enableEnhancedCivicAnswers.value();
        if (speechConfig.has_value()) j["speechConfig"] = speechConfig->toJson();
        if (thinkingConfig.has_value()) j["thinkingConfig"] = thinkingConfig->toJson();
        if (imageConfig.has_value()) j["imageConfig"] = imageConfig->toJson();
        if (mediaResolution.has_value()) j["mediaResolution"] = frenum::to_string(mediaResolution.value());

        if (responseModalities.has_value())
        {
            j["responseModalities"] = nlohmann::json::array();
            for (const auto& mod : *responseModalities)
                j["responseModalities"].push_back(frenum::to_string(mod));
        }
        
        return j;
    }
}
