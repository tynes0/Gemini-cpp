#pragma once

#ifndef GEMINI_EMBEDDINGS_API_TYPES_H
#define GEMINI_EMBEDDINGS_API_TYPES_H

#include <string>
#include <optional>

#include "nlohmann/json.hpp"
#include "frenum.h"

namespace GeminiCPP
{
    FrenumClassInNamespace(GeminiCPP, TaskType, uint8_t,
        TASK_TYPE_UNSPECIFIED, // Unset value, which will default to one of the other enum values.
        RETRIEVAL_QUERY,     // Specifies the given text is a query in a search/retrieval setting.
        RETRIEVAL_DOCUMENT,  // Specifies the given text is a document from the corpus being searched.
        SEMANTIC_SIMILARITY, // Specifies the given text will be used for STS.
        CLASSIFICATION,      // Specifies that the given text will be classified.
        CLUSTERING,          // Specifies that the embeddings will be used for clustering
        QUESTION_ANSWERING,  // Specifies that the given text will be used for question answering.
        FACT_VERIFICATION,   // Specifies that the given text will be used for fact verification.
        CODE_RETRIEVAL_QUERY // Specifies that the given text will be used for code retrieval.
    )
    
    struct EmbedRequestBody
    {
        std::optional<TaskType> taskType;
        std::string title; // Required only for RETRIEVAL_DOCUMENT
        std::optional<int> outputDimensionality; // Vector size

        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct ContentEmbedding
    {
        std::vector<float> values;

        [[nodiscard]] static ContentEmbedding fromJson(const nlohmann::json& j);
    };

    struct EmbedContentResponse
    {
        ContentEmbedding embedding;

        [[nodiscard]] static EmbedContentResponse fromJson(const nlohmann::json& j);
    };

    struct BatchEmbedContentsResponse
    {
        std::vector<ContentEmbedding> embeddings;

        [[nodiscard]] static BatchEmbedContentsResponse fromJson(const nlohmann::json& j);
    };
}

#endif // GEMINI_EMBEDDINGS_API_TYPES_H