#include "gemini/request_builder.h"
#include "gemini/client.h"

namespace GeminiCPP
{
    RequestBuilder::RequestBuilder(Client* client) : client_(client) {}

    RequestBuilder& RequestBuilder::model(Model m)
    {
        model_ = m;
        return *this;
    }

    RequestBuilder& RequestBuilder::systemInstruction(const std::string& instruction)
    {
        systemInstruction_ = instruction;
        return *this;
    }

    RequestBuilder& RequestBuilder::text(const std::string& t)
    {
        parts_.push_back(Part::Text(t));
        return *this;
    }

    RequestBuilder& RequestBuilder::image(const std::string& filepath)
    {
        parts_.push_back(Part::Media(filepath));
        return *this;
    }

    RequestBuilder& RequestBuilder::googleSearch()
    {
        Tool t;
        t.googleSearch = GoogleSearch{};
        tools_.push_back(t);
        return *this;
    }

    RequestBuilder& RequestBuilder::codeExecution()
    {
        Tool t;
        t.codeExecution = CodeExecution{};
        tools_.push_back(t);
        return *this;
    }

    RequestBuilder& RequestBuilder::cachedContent(const std::string& cacheName)
    {
        cachedContent_ = cacheName;
        return *this;
    }

    RequestBuilder& RequestBuilder::tool(const Tool& tool)
    {
        tools_.push_back(tool);
        return *this;
    }

    RequestBuilder& RequestBuilder::jsonMode()
    {
        config_.responseMimeType = "application/json";
        return *this;
    }

    RequestBuilder& RequestBuilder::temperature(float temp)
    {
        config_.temperature = temp;
        return *this;
    }

    RequestBuilder& RequestBuilder::topP(float p)
    {
        config_.topP = p;
        return *this;
    }

    RequestBuilder& RequestBuilder::topK(int k)
    {
        config_.topK = k;
        return *this;
    }

    RequestBuilder& RequestBuilder::maxTokens(int count)
    {
        config_.maxOutputTokens = count;
        return *this;
    }

    RequestBuilder& RequestBuilder::candidateCount(int count)
    {
        config_.candidateCount = count;
        return *this;
    }

    RequestBuilder& RequestBuilder::stopSequences(const std::vector<std::string>& sequences)
    {
        config_.stopSequences = sequences;
        return *this;
    }

    RequestBuilder& RequestBuilder::responseSchema(const nlohmann::json& schema)
    {
        config_.responseSchema = schema;
        if (!config_.responseMimeType.has_value())
        {
            config_.responseMimeType = "application/json";
        }
        return *this;
    }

    RequestBuilder& RequestBuilder::seed(int64_t seedValue)
    {
        config_.seed = seedValue;
        return *this;
    }

    RequestBuilder& RequestBuilder::safety(HarmCategory category, HarmBlockThreshold threshold)
    {
        safetySettings_.emplace_back(category, threshold);
        return *this;
    }

    GenerationResult RequestBuilder::generate() const
    {
        return client_->generateFromBuilder(model_, systemInstruction_, cachedContent_, parts_, config_, safetySettings_, tools_);
    }

    GenerationResult RequestBuilder::stream(const StreamCallback& callback) const
    {
        return client_->streamFromBuilder(model_, systemInstruction_, cachedContent_, parts_, config_, safetySettings_, callback, tools_);
    }

    std::future<GenerationResult> RequestBuilder::generateAsync() const
    {
        return client_->generateFromBuilderAsync(
            model_, 
            systemInstruction_,
            cachedContent_,
            parts_, 
            config_, 
            safetySettings_, 
            tools_
        );
    }

    std::future<GenerationResult> RequestBuilder::streamAsync(const StreamCallback& callback) const
    {
        return client_->streamFromBuilderAsync(
            model_, 
            systemInstruction_,
            cachedContent_,
            parts_, 
            config_, 
            safetySettings_, 
            callback, 
            tools_
        );
    }
}
    