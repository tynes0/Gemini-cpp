#include "gemini/types/tokens_api_types.h"

#include <nlohmann/json.hpp>


namespace GeminiCPP
{
    CountTokensRequestBody CountTokensRequestBody::fromJson(const nlohmann::json& j)
    {
        CountTokensRequestBody result;
        
        if (j.contains("contents"))
        {
            std::vector<Content> contents;
            contents.reserve(j["contents"].size());
            for (const auto& content : j["contents"])
                contents.push_back(Content::fromJson(content));
            result.contents = contents;
        }

        if (j.contains("generateContentRequest"))
        {
            result.generateContentRequest = GenerateContentRequestBody::fromJson(j["generateContentRequest"]);
        }

        return result;
    }

    nlohmann::json CountTokensRequestBody::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (contents.has_value())
        {
            j["contents"] = nlohmann::json::array();
            for (const auto& content : *contents)
                j["contents"].push_back(content.toJson());
        }

        if (generateContentRequest.has_value())
        {
            j["generateContentRequest"] = generateContentRequest->toJson();
        }

        return j;
    }

    CountTokensResponseBody CountTokensResponseBody::fromJson(const nlohmann::json& j)
    {
        CountTokensResponseBody result;

        if (j.contains("totalTokens"))
        {
            result.totalTokens = j["totalTokens"].get<int>();
        }

        if (j.contains("cachedContentTokenCount"))
        {
            result.cachedContentTokenCount = j["cachedContentTokenCount"].get<int>();
        }

        if (j.contains("promptTokensDetails"))
        {
            result.promptTokensDetails.reserve(j["promptTokensDetails"].size());
            for (const auto& ptd : j["promptTokensDetails"])
                result.promptTokensDetails.push_back(ModalityTokenCount::fromJson(ptd));
        }

        if (j.contains("cacheTokensDetails"))
        {
            result.cacheTokensDetails.reserve(j["cacheTokensDetails"].size());
            for (const auto& ptd : j["cacheTokensDetails"])
                result.cacheTokensDetails.push_back(ModalityTokenCount::fromJson(ptd));
        }
        
        return result;
    }

    nlohmann::json CountTokensResponseBody::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["totalTokens"] = totalTokens;
        j["cachedContentTokenCount"] = cachedContentTokenCount;
        
        j["promptTokensDetails"] = nlohmann::json::array();
        for (const auto& ptd : promptTokensDetails)
            j["promptTokensDetails"].push_back(ptd.toJson());

        j["cacheTokensDetails"] = nlohmann::json::array();
        for (const auto& ptd : cacheTokensDetails)
            j["cacheTokensDetails"].push_back(ptd.toJson());
        
        return j;
    }
}
