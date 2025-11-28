#pragma once

#ifndef GEMINI_RESPONSE_H
#define GEMINI_RESPONSE_H

#include <string>
#include <filesystem>

#include <nlohmann/json.hpp>
#include "http_status.h"
#include "types/generating_content_api_types.h"

namespace GeminiCPP
{
    template <typename T>
    struct Result
    {
        bool success = false;
        std::optional<T> value;
        std::string errorMessage;
        HttpStatusCode statusCode = HttpStatusCode::UNKNOWN;

        static Result<T> Success(T val, int code = frenum::value(HttpStatusCode::OK))
        {
            Result<T> r;
            r.success = true;
            r.value = std::move(val);
            r.statusCode = static_cast<HttpStatusCode>(code);
            return r;
        }

        static Result<T> Failure(std::string err, int code = frenum::value(HttpStatusCode::UNKNOWN))
        {
            Result<T> r;
            r.success = false;
            r.errorMessage = std::move(err);
            r.statusCode = static_cast<HttpStatusCode>(code);
            return r;
        }

        explicit operator bool() const { return success; }

        T* operator->() { return &value.value(); }
        const T* operator->() const { return &value.value(); }
        
        T& operator*() { return value.value(); }
        const T& operator*() const { return value.value(); }
    };
    
    struct GenerationResult
    {
        bool success = false;
        Content content;
        std::string errorMessage;
        HttpStatusCode statusCode = HttpStatusCode::UNKNOWN;
        FinishReason finishReason = FinishReason::FINISH_REASON_UNSPECIFIED;
        
        int inputTokens = 0;
        int outputTokens = 0;
        int totalTokens = 0;

        std::optional<GroundingMetadata> groundingMetadata;

        [[nodiscard]] std::string text() const;

        bool saveFile(const std::string& filepath, size_t blobIndex = 0) const;
        int saveAllFiles(const std::string& directory, const std::string& prefix = "output") const;

        [[nodiscard]] static GenerationResult Success(
            Content c,
            int code = frenum::value(HttpStatusCode::OK),
            int inTok = 0,
            int outTok = 0,
            FinishReason reason = FinishReason::STOP,
            std::optional<GroundingMetadata> metadata = std::nullopt
        );
        
        [[nodiscard]] static GenerationResult Failure(std::string err,
            int code = frenum::value(HttpStatusCode::UNKNOWN), FinishReason reason = FinishReason::OTHER);

        [[nodiscard]] explicit operator bool() const { return success; }
        
        [[nodiscard]] std::optional<nlohmann::json> asJson() const;

    };
}

#endif
