#pragma once

#ifndef GEMINI_SUPPORT_H
#define GEMINI_SUPPORT_H

#include <cstdint>
#include <chrono>
#include <functional>
#include <string_view>
#include <string>
#include <optional>
#include <array>

#include "http_mapped_status_code.h"

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
        HttpMappedStatusCode statusCode = HttpMappedStatusCode::STATUS_CODE_UNSPECIFIED;

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

        [[nodiscard]] std::string str() const;
        [[nodiscard]] operator std::string() const;
        // Raw data decode (binary)
        [[nodiscard]] std::vector<unsigned char> decode() const;
        // Raw string decode (text)
        [[nodiscard]] std::string decodeToString() const;

    private:
        void setFromRaw(const std::string& raw);
        
        std::string value;
    };

    // Uses RFC 3339, where generated output will always be Z-normalized and use 0, 3, 6 or 9 fractional digits.
    // Offsets other than "Z" are also accepted.
    // Examples: "2014-10-02T15:01:23Z", "2014-10-02T15:01:23.045123456Z" or "2014-10-02T15:01:23+05:30".
    struct Timestamp
    {
        Timestamp() = default;
        explicit Timestamp(const std::string& s);
        Timestamp& operator=(const std::string& s);
    
        [[nodiscard]] static Timestamp now(int digits_for_output = 3);
        [[nodiscard]] static Timestamp fromString(const std::string& s);
        [[nodiscard]] static Timestamp fromEpochWithNanos(int64_t epoch_seconds, int nanos, int digits_for_output = 3);
    
        [[nodiscard]] bool isValid() const;
        [[nodiscard]] std::string str() const;
        [[nodiscard]] operator std::string() const;
    
        // Convert to system_clock::time_point (UTC)
        [[nodiscard]] std::optional<std::chrono::system_clock::time_point> to_time_point() const;
    
    private:
        // Stored as RFC3339 Z-normalized string (or empty if invalid)
        std::string value;
        
        // -------------------------
        // Date math utilities (Howard Hinnant style)
        // -------------------------
        
        // Returns days since civil 1970-01-01, can be negative. Implementation from Howard Hinnant.
        static int64_t days_from_civil(int64_t y, unsigned m, unsigned d) noexcept;
    
        // Convert a civil date/time to epoch seconds (UTC). Returns nullopt if overflowed.
        static std::optional<int64_t> civilToEpochSeconds(int year, int month, int day, int hour, int minute, int second) noexcept;
        // Convert epoch seconds + nanos to RFC3339 Z-normalized with automatic fractional digits selection
        static std::string formatEpochZ(int64_t epoch_seconds, int nanos);
        static std::string formatEpochWithDigits(int64_t epoch_seconds, int nanos, int digits);
        // helper: floor division for negative epoch seconds
        static int64_t floor_div(int64_t x, int64_t y);
    
        // inverse of days_from_civil: produce {year, month, day}
        static std::array<int,3> civil_from_days(int64_t z) noexcept;
    };
}

#endif // GEMINI_SUPPORT_H