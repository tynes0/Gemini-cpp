#pragma once

#ifndef GEMINI_MODEL_H
#define GEMINI_MODEL_H

#include <cstdint>
#include <string_view>
#include "frenum.h"
#include "nlohmann/json.hpp"

namespace GeminiCPP
{
    /**
     * @brief Bitmask enumeration defining supported methods for a model.
     * * Used to query what capabilities (generateContent, countTokens, etc.) a specific model has.
     */
    FrenumInNamespace(GeminiCPP, GenerationMethod, uint32_t,
        GM_NONE                         = 0,
        GM_UNSPECIFIED                  = (1 <<  0),
        GM_ASYNC_BATCH_EMBED_CONTENT    = (1 <<  1), // Queues a group of EmbedContent requests for batch processing.
        GM_BATCH_EMBED_CONTENTS         = (1 <<  2), // Embed Content Creates multiple embed vectors.
        GM_BATCH_EMBED_TEXT             = (1 <<  3), // Creates multiple placed elements from the input text.
        GM_BATCH_GENERATE_CONTENT       = (1 <<  4), // Queues a group of GenerateContent requests.
        GM_COUNT_MESSAGE_TOKENS         = (1 <<  5), // Runs the model's tokenizer on a string.
        GM_COUNT_TEXT_TOKENS            = (1 <<  6), // Runs the model's tokenizer on a text.
        GM_COUNT_TOKENS                 = (1 <<  7), // Runs a model's tokenizer on the input Content.
        GM_EMBED_CONTENT                = (1 <<  8), // Creates a Content text placement vector.
        GM_EMBED_TEXT                   = (1 <<  9), // Creates a positioned element from input message.
        GM_GENERATE_CONTENT             = (1 << 10), // The model generates the response GenerateContentRequest.
        GM_GENERATE_MESSAGE             = (1 << 11), // Generates a response from input MessagePrompt. 
        GM_GENERATE_TEXT                = (1 << 12), // Generates a response from input message.
        GM_GET                          = (1 << 13), // Retrieves information about a given Model.
        GM_LIST                         = (1 << 14), // Models available through the Gemini API are listed.
        GM_PREDICT                      = (1 << 15), // Requests a prediction.
        GM_PREDICT_LONG_RUNNING         = (1 << 16), // Same as prediction but returns LRO.
        GM_STREAM_GENERATE_CONTENT      = (1 << 17) // Generates a response published from the model.
    )

    GenerationMethod operator|(GenerationMethod a, GenerationMethod b);
    GenerationMethod& operator|=(GenerationMethod& a, GenerationMethod b);

    struct GenerationMethodHelper
    {
        [[nodiscard]] static GenerationMethod fromString(const std::string& method);
        [[nodiscard]] static std::string toString(GenerationMethod method);
        [[nodiscard]] static std::string bitmaskToString(uint32_t flags);
        [[nodiscard]] static std::vector<std::string> bitmaskToStringArray(uint32_t flags);
    };
}

#endif // GEMINI_MODEL_H