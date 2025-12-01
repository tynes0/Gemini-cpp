#include "gemini/request_builder.h"
#include "gemini/client.h"

namespace GeminiCPP
{
    RequestBuilder::RequestBuilder(Client* client) 
        : client_(client), 
          model_(ModelHelper::stringRepresentation(Model::GEMINI_2_5_FLASH)),
          currentContent_(Content::User())
    {
    }

    GenerationConfig& RequestBuilder::ensureConfig()
    {
        if (!requestPrototype_.generationConfig.has_value())
        {
            requestPrototype_.generationConfig = GenerationConfig{};
        }
        return *requestPrototype_.generationConfig;
    }

    RequestBuilder& RequestBuilder::model(Model m)
    {
        model_ = ModelHelper::stringRepresentation(m);
        return *this;
    }

    RequestBuilder& RequestBuilder::systemInstruction(const std::string& instruction)
    {
        requestPrototype_.systemInstruction = Content::User().text(instruction); // System instruction is technically content
        return *this;
    }

    RequestBuilder& RequestBuilder::text(const std::string& t)
    {
        currentContent_.text(t);
        return *this;
    }

    RequestBuilder& RequestBuilder::image(const std::string& filepath)
    {
        currentContent_.image(filepath);
        return *this;
    }

    RequestBuilder& RequestBuilder::googleSearch()
    {
        Tool t;
        t.googleSearch = GoogleSearch{};
        tool(t);
        return *this;
    }

    RequestBuilder& RequestBuilder::codeExecution()
    {
        Tool t;
        t.codeExecution = CodeExecution{};
        tool(t);
        return *this;
    }

    RequestBuilder& RequestBuilder::cachedContent(const std::string& cacheName)
    {
        requestPrototype_.cachedContent = ResourceName::CachedContent(cacheName);
        return *this;
    }

    RequestBuilder& RequestBuilder::googleMaps(bool enableWidget)
    {
        Tool t;
        GoogleMaps maps;
        maps.enableWidget = enableWidget;
        t.googleMaps = maps;
        tool(t);
        return *this;
    }

    RequestBuilder& RequestBuilder::tool(const Tool& tool)
    {
        if (!requestPrototype_.tools.has_value())
        {
            requestPrototype_.tools = std::vector<Tool>{};
        }
        requestPrototype_.tools->push_back(tool);
        return *this;
    }

    RequestBuilder& RequestBuilder::location(double latitude, double longitude)
    {
        if (!requestPrototype_.toolConfig.has_value())
            requestPrototype_.toolConfig = ToolConfig{};
        
        if (!requestPrototype_.toolConfig->retrievalConfig.has_value())
            requestPrototype_.toolConfig->retrievalConfig = RetrievalConfig{};
            
        requestPrototype_.toolConfig->retrievalConfig->latLng = {
            .latitude = latitude,
            .longitude = longitude
        };
        
        return *this;
    }

    RequestBuilder& RequestBuilder::toolConfig(const ToolConfig& config)
    {
        requestPrototype_.toolConfig = config;
        return *this;
    }

    RequestBuilder& RequestBuilder::jsonMode()
    {
        ensureConfig().responseMimeType = "application/json";
        return *this;
    }

    RequestBuilder& RequestBuilder::temperature(float temp)
    {
        ensureConfig().temperature = temp;
        return *this;
    }

    RequestBuilder& RequestBuilder::topP(float p)
    {
        ensureConfig().topP = p;
        return *this;
    }

    RequestBuilder& RequestBuilder::topK(int k)
    {
        ensureConfig().topK = k;
        return *this;
    }

    RequestBuilder& RequestBuilder::maxTokens(int count)
    {
        ensureConfig().maxOutputTokens = count;
        return *this;
    }

    RequestBuilder& RequestBuilder::candidateCount(int count)
    {
        ensureConfig().candidateCount = count;
        return *this;
    }

    RequestBuilder& RequestBuilder::stopSequences(const std::vector<std::string>& sequences)
    {
        ensureConfig().stopSequences = sequences;
        return *this;
    }

    RequestBuilder& RequestBuilder::responseSchema(const nlohmann::json& schema)
    {
        ensureConfig().responseJsonSchema = schema;
        // If a schema is given, mimeType should be (I think) application/json
        if (!ensureConfig().responseMimeType.has_value())
        {
            ensureConfig().responseMimeType = "application/json";
        }
        return *this;
    }

    RequestBuilder& RequestBuilder::seed(int64_t seedValue)
    {
        ensureConfig().seed = seedValue;
        return *this;
    }

    RequestBuilder& RequestBuilder::thinking(int budget, bool includeThoughts)
    {
        ThinkingConfig tc;
        tc.includeThoughts = includeThoughts;
        tc.thinkingBudget = budget;
        
        ensureConfig().thinkingConfig = tc;
        return *this;
    }

    RequestBuilder& RequestBuilder::thinking(ThinkingLevel level, bool includeThoughts)
    {
        ThinkingConfig tc;
        tc.includeThoughts = includeThoughts;
        tc.thinkingLevel = level;
        
        ensureConfig().thinkingConfig = tc;
        return *this;
    }

    RequestBuilder& RequestBuilder::thinking(bool includeThoughts)
    {
        ThinkingConfig tc;
        tc.includeThoughts = includeThoughts;
        
        ensureConfig().thinkingConfig = tc;
        return *this;
    }

    RequestBuilder& RequestBuilder::thinking(const ThinkingConfig& thinkingConfig)
    {
        ensureConfig().thinkingConfig = thinkingConfig;
        return *this;
    }

    RequestBuilder& RequestBuilder::safety(HarmCategory category, HarmBlockThreshold threshold)
    {
        if (!requestPrototype_.safetySettings.has_value())
        {
            requestPrototype_.safetySettings = std::vector<SafetySetting>{};
        }

        SafetySetting setting;
        setting.category = category;
        setting.threshold = threshold;
        requestPrototype_.safetySettings->push_back(setting);
        
        return *this;
    }

    GenerationResult RequestBuilder::generate() const
    {
        // Copy the prototype and add the content
        GenerateContentRequestBody finalRequest = requestPrototype_;
        finalRequest.contents.push_back(currentContent_);

        return client_->generateContent(model_, finalRequest);
    }

    GenerationResult RequestBuilder::stream(const StreamCallback& callback) const
    {
        GenerateContentRequestBody finalRequest = requestPrototype_;
        finalRequest.contents.push_back(currentContent_);

        // The stream request body and the regular request body are (mostly) identical in structure,
        // but if we used a specialized structure for the stream, copying/conversion is required.
        // Client::streamGenerateContent expects StreamGenerateContentRequestBody. GenerateContentRequestBody
        // and StreamGenerateContentRequestBody are structurally identical (JSON fields).
        // Therefore, we can safely pass values (or write conversion operators within the types xD).
        
        StreamGenerateContentRequestBody streamRequest;

        streamRequest.contents = finalRequest.contents;
        streamRequest.tools = finalRequest.tools;
        streamRequest.toolConfig = finalRequest.toolConfig;
        streamRequest.safetySettings = finalRequest.safetySettings;
        streamRequest.systemInstruction = finalRequest.systemInstruction;
        streamRequest.generationConfig = finalRequest.generationConfig;
        streamRequest.cachedContent = finalRequest.cachedContent;

        return client_->streamGenerateContent(model_, streamRequest, callback);
    }

    std::future<GenerationResult> RequestBuilder::generateAsync() const
    {
        // create a copy for lambda capture
        auto self = *this;
        return std::async(std::launch::async, [self]() {
            return self.generate();
        });
    }

    std::future<GenerationResult> RequestBuilder::streamAsync(const StreamCallback& callback) const
    {
        auto self = *this;
        return std::async(std::launch::async, [self, callback]() {
            return self.stream(callback);
        });
    }
}