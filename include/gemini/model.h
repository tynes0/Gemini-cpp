#pragma once

#ifndef GEMINI_MODEL_H
#define GEMINI_MODEL_H

#include <cstdint>
#include <string_view>

namespace GeminiCPP
{
    enum class Model :  uint8_t
    {
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
    };

    [[nodiscard]] constexpr std::string_view modelStringRepresentation(Model model)
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

    [[nodiscard]] inline Model modelFromStringRepresentation(const std::string& str)
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
}

#endif // GEMINI_MODEL_H