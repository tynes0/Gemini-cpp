#pragma once

#ifndef GEMINI_TOKENS_API_TYPES_H
#define GEMINI_TOKENS_API_TYPES_H

#include <optional>
#include <vector>

#include "../types_base.h"
#include "gemini/types/generating_content_api_types.h"
#include "gemini/types/batch_api_types.h"

namespace GeminiCPP
{
    struct CountTokensRequestBody : IJsonSerializable<CountTokensRequestBody>
    {
        std::optional<std::vector<Content>> contents;
        std::optional<GenerateContentRequest> generateContentRequest;
        
        static CountTokensRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };
    
    struct CountTokensResponseBody : IJsonSerializable<CountTokensResponseBody>
    {
        int totalTokens = 0;
        int cachedContentTokenCount = 0;
        std::vector<ModalityTokenCount> promptTokensDetails;
        std::vector<ModalityTokenCount> cacheTokensDetails;
        

        [[nodiscard]] static CountTokensResponseBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };
}

#endif // GEMINI_TOKENS_API_TYPES_H