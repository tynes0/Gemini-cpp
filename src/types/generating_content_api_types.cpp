#include "gemini/types/generating_content_api_types.h"

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

    CitationSource CitationSource::fromJson(const nlohmann::json& j)
    {
        CitationSource result{};

        if (j.contains("startIndex")) result.startIndex = j["startIndex"].get<int>();
        if (j.contains("endIndex")) result.endIndex = j["endIndex"].get<int>();
        if (j.contains("uri")) result.uri = j["uri"].get<std::string>();
        if (j.contains("license")) result.license = j["license"].get<std::string>();

        return result;
    }

    nlohmann::json CitationSource::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (startIndex.has_value()) j["startIndex"] = startIndex.value();
        if (endIndex.has_value()) j["endIndex"] = endIndex.value();
        if (uri.has_value()) j["uri"] = uri.value();
        if (license.has_value()) j["license"] = license.value();
        
        return j;
    }

    CitationMetadata CitationMetadata::fromJson(const nlohmann::json& j)
    {
        CitationMetadata result{};

        if (j.contains("citationSources"))
        {
            result.citationSources.reserve(j["citationSources"].size());
            for (const auto& citationSource : j["citationSources"])
                result.citationSources.push_back(CitationSource::fromJson(citationSource));
        }

        return result;
    }

    nlohmann::json CitationMetadata::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["citationSources"] = nlohmann::json::array();
        for (const auto& citationSource : citationSources)
            j["citationSources"].push_back(citationSource.toJson());

        return j;
    }

    UrlMetadata UrlMetadata::fromJson(const nlohmann::json& j)
    {
        UrlMetadata result{};

        result.retrievedUrl = j.value("retrievedUrl", "");
        result.urlRetrievalStatus = frenum::cast<UrlRetrievalStatus>(j.value("urlRetrievalStatus", "")).value_or(UrlRetrievalStatus::URL_RETRIEVAL_STATUS_UNSPECIFIED);
        
        return result;
    }

    nlohmann::json UrlMetadata::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["retrievedUrl"] = retrievedUrl;
        j["urlRetrievalStatus"] = frenum::to_string(urlRetrievalStatus);
        
        return j;
    }

    UrlContextMetadata UrlContextMetadata::fromJson(const nlohmann::json& j)
    {
        UrlContextMetadata result{};

        if (j.contains("urlMetadata"))
        {
            result.urlMetadata.reserve(j["urlMetadata"].size());
            for (const auto& urlMetadata : j["urlMetadata"])
                result.urlMetadata.push_back(UrlMetadata::fromJson(urlMetadata));
        }
        
        return result;
    }

    nlohmann::json UrlContextMetadata::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["urlMetadata"] = nlohmann::json::array();

        for (const auto& metadata : urlMetadata)
            j["urlMetadata"].push_back(metadata.toJson());
        
        return j;
    }

    LogprobsCandidate LogprobsCandidate::fromJson(const nlohmann::json& j)
    {
        LogprobsCandidate result{};

        result.token = j.value("token", "");
        result.tokenId = j.value("tokenId", 0);
        result.logProbability = j.value("logProbability", 0.0f);

        return result;
    }

    nlohmann::json LogprobsCandidate::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["token"] = token;
        j["tokenId"] = tokenId;
        j["logProbability"] = logProbability;
        
        return j;
    }

    TopLogprobsCandidates TopLogprobsCandidates::fromJson(const nlohmann::json& j)
    {
        TopLogprobsCandidates result{};

        if (j.contains("candidates"))
        {
            result.candidates.reserve(j["candidates"].size());
            for (const auto& candidate : j["candidates"])
                result.candidates.push_back(LogprobsCandidate::fromJson(candidate));
        }
        
        return result;
    }

    nlohmann::json TopLogprobsCandidates::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["candidates"] = nlohmann::json::array();
        for (const auto& candidate : candidates)
            j["candidates"].push_back(candidate.toJson());
        
        return j;
    }

    LogprobsResult LogprobsResult::fromJson(const nlohmann::json& j)
    {
        LogprobsResult result{};

        if (j.contains("topCandidates"))
        {
            result.topCandidates.reserve(j["topCandidates"].size());
            for (const auto& candidate : j["topCandidates"])
                result.topCandidates.push_back(TopLogprobsCandidates::fromJson(candidate));
        }

        if (j.contains("chosenCandidates"))
        {
            result.chosenCandidates.reserve(j["chosenCandidates"].size());
            for (const auto& candidate : j["chosenCandidates"])
                result.chosenCandidates.push_back(LogprobsCandidate::fromJson(candidate));
        }

        result.logProbabilitySum = j.value("logProbabilitySum", 0.0f);
        
        return result;
    }

    nlohmann::json LogprobsResult::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["topCandidates"] = nlohmann::json::array();
        for (const auto& candidate : topCandidates)
            j["topCandidates"].push_back(candidate.toJson());

        j["chosenCandidates"] = nlohmann::json::array();
        for (const auto& candidate : chosenCandidates)
            j["chosenCandidates"].push_back(candidate.toJson());

        j["logProbabilitySum"] = logProbabilitySum;
        
        return j;
    }

    RetrievalMetadata RetrievalMetadata::fromJson(const nlohmann::json& j)
    {
        RetrievalMetadata result{};
        
        result.googleSearchDynamicRetrievalScore = j.value("googleSearchDynamicRetrievalScore", 0.0f);
        
        return result;
    }

    nlohmann::json RetrievalMetadata::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (googleSearchDynamicRetrievalScore.has_value())
            j["googleSearchDynamicRetrievalScore"] = googleSearchDynamicRetrievalScore.value();
        
        return j;
    }

    Segment Segment::fromJson(const nlohmann::json& j)
    {
        Segment result{};

        result.partIndex = j.value("partIndex", 0);
        result.startIndex = j.value("startIndex", 0);
        result.endIndex = j.value("endIndex", 0);
        result.text = j.value("text", "");
        
        return result;
    }

    nlohmann::json Segment::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["partIndex"] = partIndex;
        j["startIndex"] = startIndex;
        j["endIndex"] = endIndex;
        j["text"] = text;
        
        return j;
    }

    GroundingSupport GroundingSupport::fromJson(const nlohmann::json& j)
    {
        GroundingSupport result{};

        if (j.contains("groundingChunkIndices")) result.groundingChunkIndices = j["groundingChunkIndices"].get<std::vector<int>>();
        if (j.contains("confidenceScores")) result.confidenceScores = j["confidenceScores"].get<std::vector<float>>();
        if (j.contains("segment")) result.segment = Segment::fromJson(j["segment"]);
        
        return result;
    }

    nlohmann::json GroundingSupport::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["groundingChunkIndices"] = groundingChunkIndices;
        j["confidenceScores"] = confidenceScores;
        j["segment"] = segment.toJson();
        
        return j;
    }

    ReviewSnippet ReviewSnippet::fromJson(const nlohmann::json& j)
    {
        ReviewSnippet result{};

        result.reviewId = j.value("reviewId", "");
        result.googleMapsUri = j.value("googleMapsUri", "");
        result.title = j.value("title", "");
 
        return result;
    }

    nlohmann::json ReviewSnippet::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["reviewId"] = reviewId;
        j["googleMapsUri"] = googleMapsUri;
        j["title"] = title;
   
        return j;
    }

    PlaceAnswerSources PlaceAnswerSources::fromJson(const nlohmann::json& j)
    {
        PlaceAnswerSources result{};

        if (j.contains("reviewSnippets"))
        {
            result.reviewSnippets.reserve(j["reviewSnippets"].size());
            for (const auto& snip : j["reviewSnippets"])
                result.reviewSnippets.push_back(ReviewSnippet::fromJson(snip));
        }
        
        return result;
    }

    nlohmann::json PlaceAnswerSources::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        
        j["reviewSnippets"] = nlohmann::json::array();
        for (const auto& snip : reviewSnippets)
            j["reviewSnippets"].push_back(snip.toJson());
   
        return j;
    }

    Maps Maps::fromJson(const nlohmann::json& j)
    {
        Maps result{};

        result.uri = j.value("uri", "");
        result.title = j.value("title", "");
        result.text = j.value("text", "");
        result.placeId = j.value("placeId", "");

        if (j.contains("placeAnswerSources"))
            result.placeAnswerSources = PlaceAnswerSources::fromJson(j["placeAnswerSources"]);
        
        return result;
    }

    nlohmann::json Maps::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["uri"] = uri;
        j["title"] = title;
        j["text"] = text;
        j["placeId"] = placeId;
        j["placeAnswerSources"] = placeAnswerSources.toJson();

        return j;
    }

    RetrievedContext RetrievedContext::fromJson(const nlohmann::json& j)
    {
        RetrievedContext result{};

        if (j.contains("uri")) result.uri = j["uri"].get<std::string>();
        if (j.contains("title")) result.title = j["title"].get<std::string>();
        if (j.contains("text")) result.text = j["text"].get<std::string>();
        if (j.contains("fileSearchStore")) result.fileSearchStore.emplace(ResourceName::FileSearchStores(j["fileSearchStore"].get<std::string>()));
        
        return result;
    }

    nlohmann::json RetrievedContext::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (uri.has_value()) j["uri"] = uri.value();
        if (title.has_value()) j["title"] = title.value();
        if (text.has_value()) j["text"] = text.value();
        if (fileSearchStore.has_value()) j["fileSearchStore"] = fileSearchStore.value();

        return j;
    }

    Web Web::fromJson(const nlohmann::json& j)
    {
        Web result{};

        result.uri = j.value("uri", "");
        result.title = j.value("title", "");
        
        return result;
    }

    nlohmann::json Web::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["uri"] = uri;
        j["title"] = title;
        
        return j;
    }

    GroundingChunk GroundingChunk::fromJson(const nlohmann::json& j)
    {
        GroundingChunk result{};

        if (j.contains("web")) result.chunk = Web::fromJson(j["web"]);
        else if (j.contains("retrievedContext")) result.chunk = RetrievedContext::fromJson(j["retrievedContext"]);
        else if (j.contains("maps")) result.chunk = Maps::fromJson(j["maps"]);

        return result;
    }

    nlohmann::json GroundingChunk::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        std::visit([&j]<typename T>(const T& arg)
        {
            if constexpr (std::is_same_v<T, Web>) j["web"] = arg.toJson();
            else if constexpr (std::is_same_v<T, RetrievedContext>) j["retrievedContext"] = arg.toJson();
            else if constexpr (std::is_same_v<T, Maps>) j["maps"] = arg.toJson();
        }, chunk);

        return j;
    }

    SearchEntryPoint SearchEntryPoint::fromJson(const nlohmann::json& j)
    {
        SearchEntryPoint result{};

        if (j.contains("renderedContent")) result.renderedContent = j["renderedContent"].get<std::string>();
        if (j.contains("sdkBlob")) result.sdkBlob = Support::Base64String::fromBase64(j["sdkBlob"].get<std::string>());
        
        return result;
    }

    nlohmann::json SearchEntryPoint::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (renderedContent.has_value()) j["renderedContent"] = renderedContent.value();
        if (sdkBlob.has_value()) j["sdkBlob"] = sdkBlob.value().str();
        
        return j;
    }

    GroundingMetadata GroundingMetadata::fromJson(const nlohmann::json& j)
    {
        GroundingMetadata result{};

        if (j.contains("groundingChunks"))
        {
            result.groundingChunks.reserve(j["groundingChunks"].size());
            for (const auto& chunk : j["groundingChunks"])
                result.groundingChunks.push_back(GroundingChunk::fromJson(chunk));
        }
        if (j.contains("groundingSupport"))
        {
            result.groundSupports.reserve(j["groundingSupport"].size());
            for (const auto& support : j["groundingSupport"])
                result.groundSupports.push_back(GroundingSupport::fromJson(support));
        }
        if (j.contains("webSearchQueries"))
        {
            result.webSearchQueries = j["webSearchQueries"].get<std::vector<std::string>>();
        }
        if (j.contains("searchEntryPoint"))
        {
            result.searchEntryPoint = SearchEntryPoint::fromJson(j["searchEntryPoint"]);
        }
        if (j.contains("retrievalMetadata"))
        {
            result.retrievalMetadata = RetrievalMetadata::fromJson(j["retrievalMetadata"]);
        }
        if (j.contains("googleMapsWidgetContext"))
        {
            result.googleMapsWidgetContextToken = j["googleMapsWidgetContext"].get<std::string>();
        }
        
        return result;
    }

    nlohmann::json GroundingMetadata::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["groundingChunks"] = nlohmann::json::array();
        for (const auto& chunk : groundingChunks)
            j["groundingChunks"].push_back(chunk.toJson());

        j["groundSupports"] = nlohmann::json::array();
        for (const auto& support : groundSupports)
            j["groundSupports"].push_back(support.toJson());

        j["webSearchQueries"] = webSearchQueries;

        if (searchEntryPoint.has_value())
            j["searchEntryPoint"] = searchEntryPoint->toJson();

        j["retrievalMetadata"] = retrievalMetadata.toJson();

        if (googleMapsWidgetContextToken.has_value())
            j["googleMapsWidgetContextToken"] = googleMapsWidgetContextToken.value();
        
        return j;
    }

    SemanticRetrieverChunk SemanticRetrieverChunk::fromJson(const nlohmann::json& j)
    {
        SemanticRetrieverChunk result{};

        result.source = j["source"].get<std::string>();
        result.chunk = j["chunk"].get<std::string>();
        
        return result;
    }

    nlohmann::json SemanticRetrieverChunk::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["source"] = source.str();
        j["chunk"] = chunk.str();
        
        return j;
    }

    GroundingPassageId GroundingPassageId::fromJson(const nlohmann::json& j)
    {
        GroundingPassageId result{};

        result.passageId = j["passageId"].get<std::string>();
        result.partIndex = j["partIndex"].get<int>();
        
        return result;
    }

    nlohmann::json GroundingPassageId::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["passageId"] = passageId;
        j["partIndex"] = partIndex;
        
        return j;
    }

    AttributionSourceId AttributionSourceId::fromJson(const nlohmann::json& j)
    {
        AttributionSourceId result{};

        if (j.contains("groundingPassage")) result.source = GroundingPassageId::fromJson(j["groundingPassage"]);
        else if (j.contains("semanticRetrieverChunk")) result.source = SemanticRetrieverChunk::fromJson(j["semanticRetrieverChunk"]);
        
        return result;
    }

    nlohmann::json AttributionSourceId::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        std::visit([&j]<typename T>(const T& arg)
        {
            if constexpr (std::is_same_v<T, GroundingPassageId>) j["groundingPassageId"] = arg.toJson();
            else if constexpr (std::is_same_v<T, SemanticRetrieverChunk>) j["semanticRetrieverChunk"] = arg.toJson();
        }, source);

        return j;
    }

    GroundingAttribution GroundingAttribution::fromJson(const nlohmann::json& j)
    {
        GroundingAttribution result{};

        if (j.contains("sourceId")) result.sourceId = AttributionSourceId::fromJson(j["sourceId"]);
        if (j.contains("content")) result.content = Content::fromJson(j["content"]);

        return result;
    }

    nlohmann::json GroundingAttribution::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["sourceId"] = sourceId.toJson();
        j["content"] = content.toJson();
        
        return j;
    }

    ResponseCandidate ResponseCandidate::fromJson(const nlohmann::json& j)
    {
        ResponseCandidate result{};

        if (j.contains("content")) result.content = Content::fromJson(j["content"]);
        if (j.contains("finishReason")) result.finishReason = frenum::cast<FinishReason>(j["finishReason"].get<std::string>());
        if (j.contains("citationMetadata")) result.citationMetadata = CitationMetadata::fromJson(j["citationMetadata"]);
        if (j.contains("groundingMetadata")) result.groundingMetadata = GroundingMetadata::fromJson(j["groundingMetadata"]);
        if (j.contains("logprobsResult")) result.logprobsResult = LogprobsResult::fromJson(j["logprobsResult"]);
        if (j.contains("urlContextMetadata")) result.urlContextMetadata = UrlContextMetadata::fromJson(j["urlContextMetadata"]);
        if (j.contains("finishMessage")) result.finishMessage = j["finishMessage"].get<std::string>();

        result.tokenCount = j.value("tokenCount", 0);
        result.avgLogprobs = j.value("avgLogprobs", 0.0f);
        result.index = j.value("index", 0);
        
        if (j.contains("safetyRatings"))
        {
            result.safetyRatings.reserve(j["safetyRatings"].size());
            for (const auto& rating : j["safetyRatings"])
                result.safetyRatings.push_back(SafetyRating::fromJson(rating));
        }

        if (j.contains("groundingAttributions"))
        {
            result.groundingAttributions.reserve(j["groundingAttributions"].size());
            for (const auto& attr : j["groundingAttributions"])
                result.groundingAttributions.push_back(GroundingAttribution::fromJson(attr));
        }
        
        return result;
    }

    nlohmann::json ResponseCandidate::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["content"] = content.toJson();
        j["citationMetadata"] = citationMetadata.toJson();
        j["tokenCount"] = tokenCount;
        j["groundingMetadata"] = groundingMetadata.toJson();
        j["avgLogprobs"] = avgLogprobs;
        j["logprobsResult"] = logprobsResult.toJson();
        j["urlContextMetadata"] = urlContextMetadata.toJson();
        j["index"] = index;

        if (finishReason.has_value()) j["finishReason"] = frenum::to_string(finishReason.value());
        if (finishMessage.has_value()) j["finishMessage"] = finishMessage.value();

        j["safetyRatings"] = nlohmann::json::array();
        for (const auto& rating : safetyRatings)
            j["safetyRatings"].push_back(rating.toJson());

        j["groundingAttributions"] = nlohmann::json::array();
        for (const auto& attr : groundingAttributions)
            j["groundingAttributions"].push_back(attr.toJson());
        
        return j;
    }

    UsageMetadata UsageMetadata::fromJson(const nlohmann::json& j)
    {
        UsageMetadata result{};

        result.promptTokenCount = j.value("promptTokenCount", 0);
        result.cachedContentTokenCount = j.value("cachedContentTokenCount", 0);
        result.candidatesTokenCount = j.value("candidatesTokenCount", 0);
        result.toolUsePromptTokenCount = j.value("toolUsePromptTokenCount", 0);
        result.thoughtsTokenCount = j.value("thoughtsTokenCount", 0);
        result.totalTokenCount = j.value("totalTokenCount", 0);

        if (j.contains("promptTokensDetails"))
        {
            result.promptTokensDetails.reserve(j["promptTokensDetails"].size());
            for (const auto& detail : j["promptTokensDetails"])
                result.promptTokensDetails.push_back(ModalityTokenCount::fromJson(detail));
        }
        if (j.contains("cacheTokensDetails"))
        {
            result.cacheTokensDetails.reserve(j["cacheTokensDetails"].size());
            for (const auto& detail : j["cacheTokensDetails"])
                result.cacheTokensDetails.push_back(ModalityTokenCount::fromJson(detail));
        }
        if (j.contains("candidatesTokensDetails"))
        {
            result.candidatesTokensDetails.reserve(j["candidatesTokensDetails"].size());
            for (const auto& detail : j["candidatesTokensDetails"])
                result.candidatesTokensDetails.push_back(ModalityTokenCount::fromJson(detail));
        }
        if (j.contains("toolUsePromptTokensDetails"))
        {
            result.toolUsePromptTokensDetails.reserve(j["toolUsePromptTokensDetails"].size());
            for (const auto& detail : j["toolUsePromptTokensDetails"])
                result.toolUsePromptTokensDetails.push_back(ModalityTokenCount::fromJson(detail));
        }

        return result;
    }

    nlohmann::json UsageMetadata::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["promptTokenCount"] = promptTokenCount;
        j["cachedContentTokenCount"] = cachedContentTokenCount;
        j["candidatesTokenCount"] = candidatesTokenCount;
        j["toolUsePromptTokenCount"] = toolUsePromptTokenCount;
        j["thoughtsTokenCount"] = thoughtsTokenCount;
        j["totalTokenCount"] = totalTokenCount;

        j["promptTokensDetails"] = nlohmann::json::array();
        for (const auto& detail : promptTokensDetails)
            j["promptTokensDetails"].push_back(detail.toJson());

        j["cacheTokensDetails"] = nlohmann::json::array();
        for (const auto& detail : cacheTokensDetails)
            j["cacheTokensDetails"].push_back(detail.toJson());

        j["candidatesTokensDetails"] = nlohmann::json::array();
        for (const auto& detail : candidatesTokensDetails)
            j["candidatesTokensDetails"].push_back(detail.toJson());

        j["toolUsePromptTokensDetails"] = nlohmann::json::array();
        for (const auto& detail : toolUsePromptTokensDetails)
            j["toolUsePromptTokensDetails"].push_back(detail.toJson());

        return j;
    }

    PromptFeedback PromptFeedback::fromJson(const nlohmann::json& j)
    {
        PromptFeedback result{};

        if (j.contains("blockReason"))
            result.blockReason = frenum::cast<BlockReason>(j["blockReason"].get<std::string>());

        if (j.contains("safetyRatings"))
        {
            result.safetyRatings.reserve(j["safetyRatings"].size());
            for (const auto& rating : j["safetyRatings"])
                result.safetyRatings.push_back(SafetyRating::fromJson(rating));
        }
        
        return result;
    }

    nlohmann::json PromptFeedback::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (blockReason.has_value())
            j["blockReason"] = frenum::to_string(blockReason.value());

        j["safetyRatings"] = nlohmann::json::array();
        for (const auto& rating : safetyRatings)
            j["safetyRatings"].push_back(rating.toJson());
        
        return j;
    }

    GenerateContentResponse GenerateContentResponse::fromJson(const nlohmann::json& j)
    {
        GenerateContentResponse result{};

        if (j.contains("promptFeedback"))
            result.promptFeedback = PromptFeedback::fromJson(j["promptFeedback"]);

        if (j.contains("usageMetadata"))
            result.usageMetadata = UsageMetadata::fromJson(j["usageMetadata"]);

        result.modelVersion = j.value("modelVersion", "");
        result.responseId = j.value("responseId", "");

        if (j.contains("candidates"))
        {
            result.candidates.reserve(j["candidates"].size());
            for (const auto& candidate : j["candidates"])
                result.candidates.push_back(ResponseCandidate::fromJson(candidate));
        }

        return result;
    }

    nlohmann::json GenerateContentResponse::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["promptFeedback"] = promptFeedback.toJson();
        j["usageMetadata"] = usageMetadata.toJson();
        j["modelVersion"] = modelVersion;
        j["responseId"] = responseId;

        j["candidates"] = nlohmann::json::array();

        for (const auto& candidate : candidates)
            j["candidates"].push_back(candidate.toJson());
        
        return j;
    }

    GenerateContentRequestBody GenerateContentRequestBody::fromJson(const nlohmann::json& j)
    {
        GenerateContentRequestBody result{};

        if (j.contains("contents"))
        {
            result.contents.reserve(j["contents"].size());
            for (const auto& content : j["contents"])
                result.contents.push_back(Content::fromJson(content));
        }
        if (j.contains("tools"))
        {
            std::vector<Tool> tools_;
            tools_.reserve(j["tools"].size());
            for (const auto& tool : j["tools"])
                tools_.push_back(Tool::fromJson(tool));
            result.tools = tools_;
        }
        if (j.contains("toolConfig"))
        {
            result.toolConfig = ToolConfig::fromJson(j["toolConfig"]);
        }
        if (j.contains("safetySettings"))
        {
            std::vector<SafetySetting> safetySettings_;
            safetySettings_.reserve(j["safetySettings"].size());
            for (const auto& rating : j["safetySettings"])
                safetySettings_.push_back(SafetySetting::fromJson(rating));
            result.safetySettings = safetySettings_;
        }
        if (j.contains("systemInstruction"))
        {
            result.systemInstruction = Content::fromJson(j["systemInstruction"]);
        }
        if (j.contains("generationConfig"))
        {
            result.generationConfig = GenerationConfig::fromJson(j["generationConfig"]);
        }
        if (j.contains("cachedContent"))
        {
            result.cachedContent = j["cachedContent"].get<std::string>();
        }
        
        return result;
    }

    nlohmann::json GenerateContentRequestBody::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

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
            j["cachedContent"] = cachedContent->str();
        }

        return j;
    }
}
