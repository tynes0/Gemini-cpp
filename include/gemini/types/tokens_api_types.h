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
        // Optional. The input given to the model as a prompt. This field is ignored when generateContentRequest is set.
        std::optional<std::vector<Content>> contents;
        // Optional. The overall input given to the Model. This includes the prompt as well as other model steering information like system instructions,
        // and/or function declarations for function calling. Models/Contents and generateContentRequests are mutually exclusive.
        // You can either send Model + Contents or a generateContentRequest, but never both.
        std::optional<GenerateContentRequest> generateContentRequest;
        
        static CountTokensRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };
    
    struct CountTokensResponseBody : IJsonSerializable<CountTokensResponseBody>
    {
        // The number of tokens that the Model tokenizes the prompt into. Always non-negative.
        int totalTokens = 0;
        // Number of tokens in the cached part of the prompt (the cached content).
        int cachedContentTokenCount = 0;
        // Output only. List of modalities that were processed in the request input.
        std::vector<ModalityTokenCount> promptTokensDetails;
        // Output only. List of modalities that were processed in the cached content.
        std::vector<ModalityTokenCount> cacheTokensDetails;
        

        [[nodiscard]] static CountTokensResponseBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };
}

#endif // GEMINI_TOKENS_API_TYPES_H