#pragma once

#ifndef GEMINI_SUPPORT_H
#define GEMINI_SUPPORT_H

#include <functional>
#include <string_view>
#include <string>

#include "http_status.h"

namespace GeminiCPP
{
    using StreamCallback = std::function<void(std::string_view)>;
}

namespace GeminiCPP::Support
{
    struct RetryConfig
    {
        int maxRetries = 3;
        int initialDelayMs = 1000;
        int multiplier = 2;
        int maxDelayMs = 10000;
        bool enableJitter = true;
    };

    struct ApiValidationResult
    {
        bool isValid = false;
        std::string message;
        std::string reason;
        HttpStatusCode statusCode = HttpStatusCode::UNKNOWN;

        [[nodiscard]] explicit operator bool() const { return isValid; }
    };
    
    // Supported aspect ratios: 1:1, 2:3, 3:2, 3:4, 4:3, 9:16, 16:9, 21:9.
    struct AspectRatio
    {
        AspectRatio() = default;
        AspectRatio(int w_, int h_);
        AspectRatio(const std::string& s);
        AspectRatio& operator=(const std::string& s);
        [[nodiscard]] std::string str() const;
        [[nodiscard]] operator std::string() const;
        [[nodiscard]] bool isEmpty() const;
        [[nodiscard]] bool operator==(const AspectRatio& other) const;
    private:
        static bool parseString(const std::string& s, int& outW, int& outH);
        static bool isSupported(int w, int h);

        int w = 0;
        int h = 0;
    };

    // Supported values are 1K, 2K, 4K. If not specified, default value 1K.
    struct ImageSize
    {
        enum class Value : uint8_t
        {
            K1 = 1,
            K2 = 2,
            K4 = 4
        };
        
        ImageSize() = default;
        ImageSize(Value v);
        ImageSize(const std::string& s);
        ImageSize(int k);
        ImageSize& operator=(Value v);
        ImageSize& operator=(const std::string& s);
        ImageSize& operator=(int k);
        [[nodiscard]] std::string str() const;
        [[nodiscard]] operator std::string() const;
        [[nodiscard]] operator int() const;
        [[nodiscard]] bool operator==(const ImageSize& other) const;
        
    private:
        Value value = Value::K1;
    };

    /* BCP 47 format -- Valid values are:
     * de-DE, en-AU, en-GB, en-IN, en-US, es-US, fr-FR, hi-IN, pt-BR, ar-XA,
     * es-ES, fr-CA, id-ID, it-IT, ja-JP, tr-TR, vi-VN, bn-IN, gu-IN, kn-IN,
     * ml-IN, mr-IN, ta-IN, te-IN, nl-NL, ko-KR, cmn-CN, pl-PL, ru-RU, th-TH.
     */
    struct LanguageCode
    {
        static constexpr const char* Default = "en-US";

        LanguageCode() = default;
        LanguageCode(const std::string& v);
        LanguageCode& operator=(const std::string& v);
        [[nodiscard]] std::string str() const;
        [[nodiscard]] operator std::string() const;
        [[nodiscard]] bool operator==(const LanguageCode& other) const;
    private:
        void set(const std::string& v);
        
        std::string value = Default;
    };
    
    struct Base64String
    {
        Base64String() = default;
        Base64String(const Base64String& other) = default;
        Base64String& operator=(const Base64String& other) = default;
        Base64String(Base64String&& other) = default;
        Base64String& operator=(Base64String&& other) = default;
        ~Base64String() = default;
        
        [[nodiscard]] static Base64String fromBase64(const std::string& b64);

        // Raw string → Base64
        Base64String(const std::string& raw);
        Base64String& operator=(const std::string& raw);

        std::string str() const;
        operator std::string() const;
        // Raw data decode (binary)
        [[nodiscard]] std::vector<unsigned char> decode() const;
        // Raw string decode (text)
        [[nodiscard]] std::string decodeToString() const;

    private:
        void setFromRaw(const std::string& raw);
        
        std::string value;
    };

}

#endif // GEMINI_SUPPORT_H