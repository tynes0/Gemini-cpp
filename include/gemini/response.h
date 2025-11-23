#pragma once

#ifndef GEMINI_RESPONSE_H
#define GEMINI_RESPONSE_H

#include <string>
#include <filesystem>

#include <nlohmann/json.hpp>
#include "http_status.h"
#include "types.h"

namespace GeminiCPP
{
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

        [[nodiscard]] std::string text() const;

        bool saveFile(const std::string& filepath, size_t blobIndex = 0) const;
        int saveAllFiles(const std::string& directory, const std::string& prefix = "output") const;

        [[nodiscard]] static GenerationResult Success(Content c, int code = frenum::value(HttpStatusCode::OK),
            int inTok = 0, int outTok = 0, FinishReason reason = FinishReason::STOP);
        [[nodiscard]] static GenerationResult Failure(std::string err,
            int code = frenum::value(HttpStatusCode::UNKNOWN), FinishReason reason = FinishReason::OTHER);

        [[nodiscard]] explicit operator bool() const { return success; }
        
        [[nodiscard]] std::optional<nlohmann::json> asJson() const;

    };
}

#endif
