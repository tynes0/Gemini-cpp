#pragma once

#ifndef GEMINI_BATCH_API_TYPES_H
#define GEMINI_BATCH_API_TYPES_H

#include <string>
#include <optional>
#include <vector>

#include "../url.h"
#include "../types_base.h"
#include "generating_content_api_types.h"

namespace GeminiCPP
{
    struct GenerateContentRequest : IJsonSerializable<GenerateContentRequest>
    {
        ResourceName model; // Format: model/{model}
        std::vector<Content> contents;
        std::optional<std::vector<Tool>> tools;
        std::optional<ToolConfig> toolConfig;
        std::optional<std::vector<SafetySetting>> safetySettings;
        std::optional<Content> systemInstruction;
        std::optional<GenerationConfig> generationConfig;
        std::optional<std::string> cachedContent;

        [[nodiscard]] static GenerateContentRequest fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };
}

#endif // GEMINI_BATCH_API_TYPES_H