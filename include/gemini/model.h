#pragma once

#ifndef GEMINI_MODEL_H
#define GEMINI_MODEL_H

#include <cstdint>

namespace GeminiCPP
{
    enum class Model :  uint8_t
    {
        GEMINI_1_O_PRO,
        GEMINI_1_5_FLASH,
        GEMINI_1_5_PRO,
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

    constexpr std::string_view modelStringRepresentation(Model model)
    {
        switch (model)
        {
        case Model::GEMINI_1_O_PRO:                 return "gemini-1.0-pro";
        case Model::GEMINI_1_5_FLASH:               return "gemini-1.5-flash";
        case Model::GEMINI_1_5_PRO:                 return "gemini-1.5-pro";
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
            default:
            return "gemini-2.5-flash";
        }
    }
}

#endif // GEMINI_MODEL_H