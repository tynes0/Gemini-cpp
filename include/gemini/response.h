#pragma once

#ifndef GEMINI_RESPONSE_H
#define GEMINI_RESPONSE_H

#include <string>

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
        
        // TODO: token usage 
        // int inputTokens = 0;
        // int outputTokens = 0;

        [[nodiscard]] std::string text() const
        {
            std::string fullText;
            for (const auto& part : content.parts)
            {
                if (part.isText())
                {
                    fullText += part.text;
                }
            }
            return fullText;
        }

        bool saveImage(const std::string& filename) const
        {
            for (const auto& part : content.parts)
            {
                if (part.isBlob())
                {
                    auto data = Utils::base64Decode(part.inlineData);
                    std::ofstream file(filename, std::ios::binary);
                    if (file.is_open())
                    {
                        file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
                        return true;
                    }
                }
            }
            return false;
        }

        [[nodiscard]] static GenerationResult Success(Content c, int code = 200)
        {
            return {true, std::move(c), "", static_cast<HttpStatusCode>(code)};
        }

        [[nodiscard]] static GenerationResult Failure(std::string err, int code = 0)
        {
            return {false, Content{}, std::move(err), static_cast<HttpStatusCode>(code)};
        }

        [[nodiscard]] explicit operator bool() const { return success; }
    };
}

#endif
