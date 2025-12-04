#pragma once

#ifndef GEMINI_RESPONSE_H
#define GEMINI_RESPONSE_H

#include <string>

#include <nlohmann/json.hpp>
#include "http_mapped_status_code.h"
#include "types/generating_content_api_types.h"

namespace GeminiCPP
{
    /**
     * @brief A generic wrapper for API operation results.
     * * @tparam T The type of the value returned on success.
     */
    template <typename T>
    struct Result
    {
        bool success = false;
        std::optional<T> value;
        std::string errorMessage;
        HttpMappedStatusCode statusCode = HttpMappedStatusCode::STATUS_CODE_UNSPECIFIED;

        static Result<T> Success(T val, int code = frenum::value(HttpMappedStatusCode::OK))
        {
            Result<T> r;
            r.success = true;
            r.value = std::move(val);
            r.statusCode = static_cast<HttpMappedStatusCode>(code);
            return r;
        }

        static Result<T> Failure(std::string err, int code = frenum::value(HttpMappedStatusCode::STATUS_CODE_UNSPECIFIED))
        {
            Result<T> r;
            r.success = false;
            r.errorMessage = std::move(err);
            r.statusCode = static_cast<HttpMappedStatusCode>(code);
            return r;
        }

        explicit operator bool() const { return success; }

        T* operator->() { return &value.value(); }
        const T* operator->() const { return &value.value(); }
        
        T& operator*() { return value.value(); }
        const T& operator*() const { return value.value(); }
    };
    
    /**
     * @brief Represents the result of a content generation request.
     * * Contains the generated content, usage metadata, and helper methods to extract
     * text or save binary blobs (like images/audio).
     */
    struct GenerationResult
    {
        bool success = false;
        /// @brief The generated content (text, blobs, function calls).
        Content content;
        std::string errorMessage;
        HttpMappedStatusCode statusCode = HttpMappedStatusCode::STATUS_CODE_UNSPECIFIED;
        FinishReason finishReason = FinishReason::FINISH_REASON_UNSPECIFIED;
        
        // Token Usage
        int inputTokens = 0;
        int outputTokens = 0;
        int totalTokens = 0;

        std::optional<GroundingMetadata> groundingMetadata;

        /**
         * @brief Helper to extract all text parts from the content as a single string.
         * @return Concatenated text from the response.
         */
        [[nodiscard]] std::string text() const;

        /**
         * @brief Saves a binary blob (e.g. image) from the response to a file.
         * @param filepath The path to save the file.
         * @param blobIndex The index of the blob if multiple are present (default 0).
         * @return true if successful.
         */
        bool saveFile(const std::string& filepath, size_t blobIndex = 0) const;

        /**
         * @brief Saves all binary blobs in the response to a directory.
         * @param directory The target directory.
         * @param prefix Filename prefix (e.g., "output" -> "output_0.png").
         * @return Number of files saved.
         */
        int saveAllFiles(const std::string& directory, const std::string& prefix = "output") const;

        [[nodiscard]] static GenerationResult Success(
            Content c,
            int code = frenum::value(HttpMappedStatusCode::OK),
            int inTok = 0,
            int outTok = 0,
            FinishReason reason = FinishReason::STOP,
            std::optional<GroundingMetadata> metadata = std::nullopt
        );
        
        [[nodiscard]] static GenerationResult Failure(std::string err,
            int code = frenum::value(HttpMappedStatusCode::STATUS_CODE_UNSPECIFIED), FinishReason reason = FinishReason::OTHER);

        [[nodiscard]] explicit operator bool() const { return success; }
        
        /**
         * @brief Tries to parse the text response as JSON.
         * @details Attempts to find JSON objects/arrays even if surrounded by markdown.
         */
        [[nodiscard]] std::optional<nlohmann::json> asJson() const;

    };
}

#endif