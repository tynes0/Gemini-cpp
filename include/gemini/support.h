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
    /// @brief Callback type for handling streamed text chunks.
    using StreamCallback = std::function<void(std::string_view)>;
}

namespace GeminiCPP::Support
{
    /**
     * @brief Configuration for HTTP request retries.
     */
    struct RetryConfig
    {
        int maxRetries = 3;         ///< Maximum number of retry attempts.
        int initialDelayMs = 1000;  ///< Initial delay in milliseconds before the first retry.
        int multiplier = 2;         ///< Multiplier for exponential backoff.
        int maxDelayMs = 10000;     ///< Maximum delay in milliseconds.
        bool enableJitter = true;   ///< Whether to add random jitter to the delay.
    };

    /**
     * @brief Result of an API key validation check.
     */
    struct ApiValidationResult
    {
        bool isValid = false;       ///< True if the key is valid.
        std::string message;        ///< Status message (e.g., "OK" or error details).
        std::string reason;         ///< Detailed reason for failure.
        HttpMappedStatusCode statusCode = HttpMappedStatusCode::STATUS_CODE_UNSPECIFIED; ///< HTTP status code returned.

        [[nodiscard]] explicit operator bool() const { return isValid; }
    };
    
    /**
     * @brief Represents an image aspect ratio.
     * Supported ratios: 1:1, 2:3, 3:2, 3:4, 4:3, 9:16, 16:9, 21:9.
     */
    class AspectRatio
    {
    public:
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

    /**
     * @brief Represents the size of an image for generation.
     * Supported values are 1K, 2K, 4K.
     */
    class ImageSize
    {
    public:
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

    /**
     * @brief Helper class for BCP 47 language codes (e.g., "en-US", "tr-TR").
     */
    class LanguageCode
    {
    public:
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
    
    /**
     * @brief Wrapper for Base64 encoded strings to differentiate them from raw strings.
     */
    class Base64String
    {
    public:
        Base64String() = default;
        Base64String(const Base64String& other) = default;
        Base64String& operator=(const Base64String& other) = default;
        Base64String(Base64String&& other) = default;
        Base64String& operator=(Base64String&& other) = default;
        ~Base64String() = default;
        
        /**
         * @brief Creates an instance from an already encoded base64 string.
         */
        [[nodiscard]] static Base64String fromBase64(const std::string& b64);

        /**
         * @brief Creates an instance by encoding a raw string into base64.
         */
        Base64String(const std::string& raw);
        Base64String& operator=(const std::string& raw);

        [[nodiscard]] std::string str() const;
        [[nodiscard]] operator std::string() const;
        
        /// @brief Decodes the base64 string into a byte vector.
        [[nodiscard]] std::vector<unsigned char> decode() const;
        
        /// @brief Decodes the base64 string into a text string.
        [[nodiscard]] std::string decodeToString() const;

    private:
        void setFromRaw(const std::string& raw);
        
        std::string value;
    };

    /**
     * @brief Represents a timestamp in RFC 3339 format.
     */
    class Timestamp
    {
    public:
        Timestamp() = default;
        explicit Timestamp(const std::string& s);
        Timestamp& operator=(const std::string& s);
    
        [[nodiscard]] static Timestamp now(int digits_for_output = 3);
        [[nodiscard]] static Timestamp fromString(const std::string& s);
        [[nodiscard]] static Timestamp fromEpochWithNanos(int64_t epoch_seconds, int nanos, int digits_for_output = 3);
    
        [[nodiscard]] bool isValid() const;
        [[nodiscard]] std::string str() const;
        [[nodiscard]] operator std::string() const;
    
        [[nodiscard]] std::optional<std::chrono::system_clock::time_point> to_time_point() const;
    
    private:
        std::string value;
        // ... (private helpers omitted for brevity in docs)
        static int64_t days_from_civil(int64_t y, unsigned m, unsigned d) noexcept;
        static std::optional<int64_t> civilToEpochSeconds(int year, int month, int day, int hour, int minute, int second) noexcept;
        static std::string formatEpochZ(int64_t epoch_seconds, int nanos);
        static std::string formatEpochWithDigits(int64_t epoch_seconds, int nanos, int digits);
        static int64_t floor_div(int64_t x, int64_t y);
        static std::array<int,3> civil_from_days(int64_t z) noexcept;
    };

    /**
     * @brief Represents a FieldMask for update operations (JSON-style paths).
     */
    class FieldMask
    {
    public:
        FieldMask() = default;

        explicit FieldMask(const std::string& jsonMaskString);
        explicit FieldMask(const std::vector<std::string>& paths);

        [[nodiscard]] std::string toJsonString() const;
        void fromJsonString(const std::string& json);
        operator std::string() const;
        
        void addPath(const std::string& path);
        void setPaths(const std::vector<std::string>& paths);
        [[nodiscard]] const std::vector<std::string>& paths() const;

    private:
        std::vector<std::string> paths_;
        
        static std::string trim(const std::string& s);
        static std::string toLowerCamelCase(const std::string& field);
        void addProcessedPath(const std::string& raw);
    };
}

#endif // GEMINI_SUPPORT_H