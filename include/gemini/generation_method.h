#pragma once

#ifndef GEMINI_GENERATION_METHOD_H
#define GEMINI_GENERATION_METHOD_H

#include <string>
#include <cstdint>

#include "frenum.h"

namespace GeminiCPP
{
    FrenumInNamespace(GeminiCPP, GenerationMethod, uint32_t,
        GM_NONE                         = 0,
        GM_UNSPECIFIED                  = (1 <<  0),
        GM_ASYNC_BATCH_EMBED_CONTENT    = (1 <<  1), // Queues a group of EmbedContent requests for batch processing.
        GM_BATCH_EMBED_CONTENTS         = (1 <<  2), // Embed Content Creates multiple embed vectors from the Content input, which consists of a series of strings represented as Request objects.
        GM_BATCH_EMBED_TEXT             = (1 <<  3), // In a synchronous call, creates multiple placed elements from the input text given to the model.
        GM_BATCH_GENERATE_CONTENT       = (1 <<  4), // Queues a group of GenerateContent requests for batch processing.
        GM_COUNT_MESSAGE_TOKENS         = (1 <<  5), // Runs the model's tokenizer on a string and returns the number of tokens.
        GM_COUNT_TEXT_TOKENS            = (1 <<  6), // Runs the model's tokenizer on a text and returns the number of tokens.
        GM_COUNT_TOKENS                 = (1 <<  7), // Runs a model's tokenizer on the input Content and returns the number of tokens.
        GM_EMBED_CONTENT                = (1 <<  8), // Creates a Content text placement vector from the input using the specified Gemini placement model.
        GM_EMBED_TEXT                   = (1 <<  9), // Creates a positioned element from the model when given an input message.
        GM_GENERATE_CONTENT             = (1 << 10), // Given input, the model generates the response GenerateContentRequest.
        GM_GENERATE_MESSAGE             = (1 << 11), // Generates a response from the model when the input MessagePrompt is given. 
        GM_GENERATE_TEXT                = (1 << 12), // When given an input message, it generates a response from the model.
        GM_GET                          = (1 << 13), // Retrieves information about a given Model, such as version number, token limits, parameters, and other metadata.
        GM_LIST                         = (1 << 14), // Models available through the Gemini API are listed.
        GM_PREDICT                      = (1 << 15), // Requests a prediction.
        GM_PREDICT_LONG_RUNNING         = (1 << 16), // Same as prediction but returns LRO.
        GM_STREAM_GENERATE_CONTENT      = (1 << 17) // Generates a response published from the model using the GenerateContentRequest input.
    )

    GenerationMethod operator|(GenerationMethod a, GenerationMethod b);
    GenerationMethod& operator|=(GenerationMethod& a, GenerationMethod b);

    struct GenerationMethodHelper
    {
        [[nodiscard]] static GenerationMethod fromString(const std::string& method);
        [[nodiscard]] static std::string toString(GenerationMethod method);
        [[nodiscard]] static std::string bitmaskToString(uint32_t flags);
    };
}

#endif // GEMINI_GENERATION_METHOD_H