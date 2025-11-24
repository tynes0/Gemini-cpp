#pragma once

#ifndef GEMINI_TYPES_H
#define GEMINI_TYPES_H

#include <string>
#include <functional>
#include <optional>
#include <variant>

#include <frenum.h>
#include <nlohmann/json.hpp>

#include "http_status.h"

namespace GeminiCPP
{
    struct FunctionDeclaration
    {
        std::string name;
        std::string description;
        nlohmann::json parameters;

        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct GoogleSearch
    {
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct Tool
    {
        std::vector<FunctionDeclaration> functionDeclarations;
        std::optional<GoogleSearch> googleSearch;

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

    struct TextData
    {
        std::string text;
    };
    struct BlobData
    {
        std::string mimeType;
        std::string data;
    };

    struct FileData
    {
        std::string mimeType; 
        std::string fileUri;
    };

    struct Part
    {
        using VariantType = std::variant<std::monostate, TextData, BlobData, FileData, FunctionCall, FunctionResponse>;
        
        VariantType content;

        [[nodiscard]] bool isText() const;
        [[nodiscard]] bool isBlob() const; // Inline Base64
        [[nodiscard]] bool isFileData() const; // URI Reference
        [[nodiscard]] bool isFunctionCall() const;
        [[nodiscard]] bool isFunctionResponse() const;

        [[nodiscard]] const std::string* getText() const;
        [[nodiscard]] const BlobData* getBlob() const;
        [[nodiscard]] const FileData* getFileData() const;
        [[nodiscard]] const FunctionCall* getFunctionCall() const;
        [[nodiscard]] const FunctionResponse* getFunctionResponse() const;

        [[nodiscard]] static Part Text(std::string t);
        [[nodiscard]] static Part Media(const std::string& filepath, const std::string& customMimeType = "");
        [[nodiscard]] static Part Uri(std::string fileUri, std::string mimeType);
        [[nodiscard]] static Part Call(FunctionCall call);
        [[nodiscard]] static Part Response(FunctionResponse resp);

        [[nodiscard]] nlohmann::json toJson() const;
    };

    // Who's talking?
    enum class Role : uint8_t
    {
        USER,
        MODEL,
        FUNCTION
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
        Content& fileUri(const std::string& uri, const std::string& mimeType);
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

    struct EmbedConfig
    {
        std::optional<TaskType> taskType;
        std::string title; // Required only for RETRIEVAL_DOCUMENT
        std::optional<int> outputDimensionality; // Vector size

        [[nodiscard]] nlohmann::json toJson() const;
    };

    FrenumClassInNamespace(GeminiCPP, HarmCategory, uint8_t,
        HARM_CATEGORY_UNSPECIFIED, // Default, not used.

        // Text Categories
        HARM_CATEGORY_HATE_SPEECH, // Content that promotes violence or hatred.
        HARM_CATEGORY_DANGEROUS_CONTENT, // Content that promotes dangerous activities.
        HARM_CATEGORY_HARASSMENT, // Content that contains harassment, threats or bullying.
        HARM_CATEGORY_SEXUALLY_EXPLICIT, // Sexual content.
        HARM_CATEGORY_CIVIC_INTEGRITY, // (Deprecated) Election security etc.

        // Image Categories
        HARM_CATEGORY_IMAGE_HATE,
        HARM_CATEGORY_IMAGE_DANGEROUS_CONTENT,
        HARM_CATEGORY_IMAGE_HARASSMENT,
        HARM_CATEGORY_IMAGE_SEXUALLY_EXPLICIT,

        // Special Categories
        HARM_CATEGORY_JAILBREAK // Prompts that attempt to bypass security filters.
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

    struct RetryConfig
    {
        int maxRetries = 3;
        int initialDelayMs = 1000;
        int multiplier = 2;
        int maxDelayMs = 10000;
        bool enableJitter = true;
    };

    FrenumClassInNamespace(GeminiCPP, FileState, uint8_t,
        STATE_UNSPECIFIED,
        PROCESSING, // Processing file (Not immediately available)
        ACTIVE,     // Ready to use
        FAILED      // Processing error
    )

    struct File
    {
        std::string name;          // ID in "files/..." format
        std::string displayName;   // User-given name
        std::string mimeType;      // "image/jpeg", "video/mp4" etc.
        std::string sizeBytes;     // String income (API standard)
        std::string createTime;
        std::string updateTime;
        std::string expirationTime;
        std::string sha256Hash;
        std::string uri;           // "https://..." (This is used in content)
        FileState state = FileState::STATE_UNSPECIFIED;
        
        std::string errorMsg; 

        [[nodiscard]] static File fromJson(const nlohmann::json& j);
        [[nodiscard]] std::string toString() const;
    };
    
    struct ListFilesResponse
    {
        std::vector<File> files;
        std::string nextPageToken;

        [[nodiscard]] static ListFilesResponse fromJson(const nlohmann::json& j);
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

    enum GenerationMethod : uint32_t
    {
        GM_NONE                         = 0,
        GM_UNSPECIFIED                  = (1 <<  0),
        GM_ASYNC_BATCH_EMBED_CONTENT    = (1 <<  1), // Queues a group of EmbedContent requests for batch processing.
        GM_BATCH_EMBED_CONTENTS         = (1 <<  2), // Embed Content Creates multiple embed vectors from the Content input, which consists of a series of strings represented as Request objects.
        GM_BATCH_EMBED_TEXT             = (1 <<  3), // In a synchronous call, creates multiple placed elements from the input text given to the model.
        GM_BATCH_GENERATE_CONTENT       = (1 <<  4), // Queues a group of GenerateContent requests for batch processing.
        GM_COUNT_MESSAGE_TOKENS         = (1 <<  5), // Runs the model's tokenizer on a string and returns the number of tokens.
        GM_COUNT_TEXT_TOKENS            = (1 <<  6), // Runs the model's tokenizer on a text and returns the number of tokens.
        GM_COUNT_TOKENS                 = (1 <<  7), // Runs a model's tokenizer on the input Content and returns the number of tokens.
        GM_EMBED_CONTENT                = (1 <<  8), // Creates a Content text placement vector from the input using the specified Gemini placement model.
        GM_EMBED_TEXT                   = (1 <<  9), // Creates a positioned element from the model when given an input message.
        GM_GENERATE_CONTENT             = (1 << 10), // Given input, the model generates the response GenerateContentRequest.
        GM_GENERATE_MESSAGE             = (1 << 11), // Generates a response from the model when the input MessagePrompt is given. 
        GM_GENERATE_TEXT                = (1 << 12), // When given an input message, it generates a response from the model.
        GM_GET                          = (1 << 13), // Retrieves information about a given Model, such as version number, token limits, parameters, and other metadata.
        GM_LIST                         = (1 << 14), // Models available through the Gemini API are listed.
        GM_PREDICT                      = (1 << 15), // Requests a prediction.
        GM_PREDICT_LONG_RUNNING         = (1 << 16), // Same as prediction but returns LRO.
        GM_STREAM_GENERATE_CONTENT      = (1 << 17) // Generates a response published from the model using the GenerateContentRequest input.
    };

    inline GenerationMethod operator|(GenerationMethod a, GenerationMethod b)
    {
        return static_cast<GenerationMethod>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    
    inline GenerationMethod& operator|=(GenerationMethod& a, GenerationMethod b)
    {
        a = a | b;
        return a;
    }

    struct GenerationMethodHelper
    {
        [[nodiscard]] static GenerationMethod fromString(const std::string& method);
        [[nodiscard]] static std::string toString(GenerationMethod method);
        [[nodiscard]] static std::string bitmaskToString(uint32_t flags);
    };

    struct ModelInfo
    {
        std::string name;
        std::string version;
        std::string displayName;
        std::string description;
        int inputTokenLimit = 0;
        int outputTokenLimit = 0;
        double temperature = 0.0;
        double topP = 0.0;
        int topK = 0;
        uint32_t supportedGenerationMethods = GM_NONE;

        [[nodiscard]] bool supports(GenerationMethod method) const
        {
            return (supportedGenerationMethods & method) != 0;
        }
        
        [[nodiscard]] static ModelInfo fromJson(const nlohmann::json& j);
        [[nodiscard]] std::string toString() const;
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

    struct TokenCountResponse
    {
        int totalTokens = 0;

        [[nodiscard]] static TokenCountResponse fromJson(const nlohmann::json& j);
    };
    
    using StreamCallback = std::function<void(std::string_view)>;

    struct ApiValidationResult
    {
        bool isValid = false;
        std::string message;
        std::string reason;
        HttpStatusCode statusCode = HttpStatusCode::UNKNOWN;

        [[nodiscard]] explicit operator bool() const { return isValid; }
    };

    struct CachedContentUsage
    {
        int tokenCount = 0;
    };

    struct CachedContent
    {
        std::string name; // ID: "cachedContents/..."
        std::string model;
        std::string displayName;
        std::string createTime;
        std::string updateTime;
        std::string expireTime;
        std::string ttl; // Duration (e.g. "3600s")

        CachedContentUsage usage;

        // Content to be used when creating the cache (It will only be filled at creation time)
        std::optional<std::string> systemInstruction;
        std::vector<Content> contents;
        std::vector<Tool> tools;

        [[nodiscard]] static CachedContent fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct ListCachedContentsResponse
    {
        std::vector<CachedContent> cachedContents;
        std::string nextPageToken;

        [[nodiscard]] static ListCachedContentsResponse fromJson(const nlohmann::json& j);
    };
} // namespace GeminiCPP
#endif // GEMINI_TYPES_H