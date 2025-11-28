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
        TASK_TYPE_UNSPECIFIED,
        RETRIEVAL_QUERY,     // Search query (Used when asking a question)
        RETRIEVAL_DOCUMENT,  // Search document (used when saving to database)
        SEMANTIC_SIMILARITY, // To measure the similarity between two texts
        CLASSIFICATION,      // For classification tasks
        CLUSTERING,          // For clustering operations
        QUESTION_ANSWERING,  // Answering questions (Sometimes used)
        FACT_VERIFICATION    // Verification
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