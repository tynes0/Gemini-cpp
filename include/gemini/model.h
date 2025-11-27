#pragma once

#ifndef GEMINI_MODEL_H
#define GEMINI_MODEL_H

#include <cstdint>
#include <string_view>
#include "frenum.h"
#include "nlohmann/json.hpp"

#include "generation_method.h"

namespace GeminiCPP
{
    FrenumClassInNamespace(GeminiCPP, Model,  uint8_t,
        GEMINI_2_0_FLASH,
        GEMINI_2_0_FLASH_LITE,
        GEMINI_2_5_FLASH,
        GEMINI_2_5_FLASH_IMAGE,
        GEMINI_2_5_FLASH_TTS,
        GEMINI_2_5_FLASH_LITE,
        GEMINI_2_5_PRO,
        GEMINI_2_5_PRO_TTS,
        GEMINI_3_0_PRO_PREVIEW,
        GEMINI_3_0_PRO_IMAGE_PREVIEW,

        GEMINI_FLASH_LATEST,
        GEMINI_PRO_LATEST
    )

    struct ModelHelper
    {
        [[nodiscard]] static std::string_view stringRepresentation(Model model);
        [[nodiscard]] static Model fromStringRepresentation(const std::string& str);
    };
    
    struct ModelInfo
    {
        std::string name;
        std::string version;
        std::string displayName;
        std::string description;
        int inputTokenLimit = 0;
        int outputTokenLimit = 0;
        double temperature = 0.0;
        double topP = 0.0;
        int topK = 0;
        uint32_t supportedGenerationMethods = GM_NONE;

        [[nodiscard]] bool supports(GenerationMethod method) const;
        
        [[nodiscard]] static ModelInfo fromJson(const nlohmann::json& j);
        [[nodiscard]] std::string toString() const;
    };
}

#endif // GEMINI_MODEL_H