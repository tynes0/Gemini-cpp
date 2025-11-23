#pragma once

#ifndef GEMINI_TYPES_H
#define GEMINI_TYPES_H

#include <string>
#include <functional>
#include <optional>

#include <frenum.h>
#include <nlohmann/json.hpp>


namespace GeminiCPP
{
    // Who's talking?
    enum class Role : uint8_t
    {
        USER,
        MODEL,
        FUNCTION
    };

    struct FunctionDeclaration
    {
        std::string name;
        std::string description;
        nlohmann::json parameters;

        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct Tool
    {
        std::vector<FunctionDeclaration> functionDeclarations;

        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct FunctionCall
    {
        std::string name;
        nlohmann::json args;

        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct FunctionResponse
    {
        std::string name;
        nlohmann::json response;

        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct Part
    {
        std::string text;
        std::string inlineData;
        std::string mimeType;

        std::optional<FunctionCall> functionCall;
        std::optional<FunctionResponse> functionResponse;

        bool isText() const;
        bool isBlob() const;
        bool isFunctionCall() const;
        bool isFunctionResponse() const;

        [[nodiscard]] static Part Text(std::string t);
        [[nodiscard]] static Part Media(const std::string& filepath, const std::string& customMimeType = "");
        [[nodiscard]] static Part Call(FunctionCall call);
        [[nodiscard]] static Part Response(FunctionResponse resp);

        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct Content
    {
        Role role = Role::USER;
        std::vector<Part> parts;

        [[nodiscard]] static Content User();
        [[nodiscard]] static Content Model();
        [[nodiscard]] static Content Function();
        
        Content& text(const std::string& t);
        Content& image(const std::string& filepath);
        Content& file(const std::string& filepath);
        Content& media(const std::string& filepath, const std::string& mimeType = "");
        Content& functionResponse(std::string name, nlohmann::json responseContent);

        [[nodiscard]] nlohmann::json toJson() const;
        [[nodiscard]] static Content fromJson(const nlohmann::json& j);
    };
    
    struct GenerationConfig
    {
        // Optional fields to allow API defaults when not set
        std::optional<std::string> responseMimeType;
        std::optional<std::vector<std::string>> stopSequences;
        std::optional<float> temperature;
        std::optional<float> topP;
        std::optional<int> topK;
        std::optional<int> candidateCount;
        std::optional<int> maxOutputTokens;
        std::optional<bool> responseLogprobs;
        std::optional<int> logprobs;
        std::optional<float> presencePenalty;
        std::optional<float> frequencyPenalty;
        std::optional<int64_t> seed;
        std::optional<nlohmann::json> responseSchema; // For structured output schemas

        [[nodiscard]] nlohmann::json toJson() const;
    };

    FrenumClassInNamespace(GeminiCPP, HarmCategory, uint8_t, 
            HARM_CATEGORY_UNSPECIFIED,             // Default, not used.
        
            // Text Categories
            HARM_CATEGORY_HATE_SPEECH,             // Content that promotes violence or hatred.
            HARM_CATEGORY_DANGEROUS_CONTENT,       // Content that promotes dangerous activities.
            HARM_CATEGORY_HARASSMENT,              // Content that contains harassment, threats or bullying.
            HARM_CATEGORY_SEXUALLY_EXPLICIT,       // Sexual content.
            HARM_CATEGORY_CIVIC_INTEGRITY,         // (Deprecated) Election security etc.
        
            // Image Categories
            HARM_CATEGORY_IMAGE_HATE,
            HARM_CATEGORY_IMAGE_DANGEROUS_CONTENT,
            HARM_CATEGORY_IMAGE_HARASSMENT,
            HARM_CATEGORY_IMAGE_SEXUALLY_EXPLICIT,
        
            // Special Categories
            HARM_CATEGORY_JAILBREAK                // Prompts that attempt to bypass security filters.
    )

    FrenumClassInNamespace(GeminiCPP, HarmBlockThreshold, uint8_t,
            UNSPECIFIED,             // Unspecified.
            BLOCK_LOW_AND_ABOVE,     // Block low risk and above (Very Strict).
            BLOCK_MEDIUM_AND_ABOVE,  // Block medium risk and above (Default).
            BLOCK_ONLY_HIGH,         // Only block high risk.
            BLOCK_NONE,              // Don't block anything (Risky).
            OFF                      // Turn off the filter completely.
    )

    struct SafetySetting
    {
        HarmCategory category;
        HarmBlockThreshold threshold;

        [[nodiscard]] static std::string categoryToString(HarmCategory cat);
        [[nodiscard]] static std::string thresholdToString(HarmBlockThreshold thr);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    FrenumClassInNamespace(GeminiCPP, FinishReason, uint8_t,
            FINISH_REASON_UNSPECIFIED,// Default.
            STOP,                   // Natural ending or stopping sequence.
            MAX_TOKENS,             // The maximum token limit has been reached.
            SAFETY,                 // Stopped due to security breach.
            RECITATION,             // Potential for copyright/quotation infringement.
            OTHER,                  // Other reasons.
            BLOCKLIST,              // It stopped because it contained banned words.
            PROHIBITED_CONTENT,     // Potential for prohibited content.
            SPII,                   // It stopped because it contained Sensitive Personal Data (SPII).
            MALFORMED_FUNCTION_CALL,// The function call generated by the model is invalid.
            MODEL_ARMOR,            // The model is blocked by the Armor layer.

            // The special case I added (it does not come from the API, it is generated by the Client)
            PROMPT_BLOCKED
    )

    struct FinishReasonHelper
    {
        [[nodiscard]] static FinishReason fromString(const std::string& reason);
        [[nodiscard]] static std::string toString(FinishReason reason);
    };

    struct ModelInfo
    {
        std::string name;
        std::string version;
        std::string displayName;
        std::string description;
        int inputTokenLimit = 0;
        int outputTokenLimit = 0;
        std::vector<std::string> supportedGenerationMethods;
        double temperature = 0.0;
        double topP = 0.0;
        int topK = 0;

        [[nodiscard]] static ModelInfo fromJson(const nlohmann::json& j);
        [[nodiscard]] std::string toString() const;
    };
    
    using StreamCallback = std::function<void(std::string_view)>;
} // namespace GeminiCPP
#endif // GEMINI_TYPES_H