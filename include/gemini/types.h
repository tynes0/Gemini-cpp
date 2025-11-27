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
#include "duration.h"

namespace GeminiCPP
{
    FrenumClassInNamespace(GeminiCPP, FunctionCallingMode, uint8_t,
        MODE_UNSPECIFIED, // Unspecified function calling mode. This value should not be used.
        AUTO, // Default model behavior, model decides to predict either a function call or a natural language response.
        ANY, // Model is constrained to always predicting a function call only. If "allowedFunctionNames" are set, the predicted function call will be limited to any one of "allowedFunctionNames", else the predicted function call will be any one of the provided "functionDeclarations".
        NONE, // Model will not predict any function call. Model behavior is same as when not passing any function declarations.
        VALIDATED // Model decides to predict either a function call or a natural language response, but will validate function calls with constrained decoding. If "allowedFunctionNames" are set, the predicted function call will be limited to any one of "allowedFunctionNames", else the predicted function call will be any one of the provided "functionDeclarations".
    )

    struct FunctionCallingConfig
    {
        std::optional<FunctionCallingMode> mode;
        std::optional<std::vector<std::string>> allowedFunctionNames;

        [[nodiscard]] static FunctionCallingConfig fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
        
    };
    
    struct LatLng
    {
        double latitude;
        double longitude;

        [[nodiscard]] static LatLng fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct RetrievalConfig
    {
        std::optional<LatLng> latLng;
        std::optional<std::string> languageCode;

        [[nodiscard]] static RetrievalConfig fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct ToolConfig
    {
        std::optional<FunctionCallingConfig> functionCallingConfig;
        std::optional<RetrievalConfig> retrievalConfig;

        [[nodiscard]] static ToolConfig fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    // TODO: Update all the tools
    struct FunctionDeclaration
    {
        std::string name;
        std::string description;
        nlohmann::json parameters;

        [[nodiscard]] static FunctionDeclaration fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct GoogleSearch
    {
        [[nodiscard]] static GoogleSearch fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct GoogleMaps
    {
        bool enableWidget = false;
        
        [[nodiscard]] static GoogleMaps fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };
    
    struct CodeExecution
    {
        [[nodiscard]] static CodeExecution fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct Tool
    {
        std::vector<FunctionDeclaration> functionDeclarations;
        std::optional<GoogleSearch> googleSearch;
        std::optional<CodeExecution> codeExecution;
        std::optional<GoogleMaps> googleMaps;

        [[nodiscard]] static Tool fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct FunctionResponseBlob
    {
        std::string mimeType;
        std::string data; // Base64

        [[nodiscard]] static FunctionResponseBlob fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct FunctionResponsePart
    {
        using DataType = std::variant<
            std::monostate,
            FunctionResponseBlob
        >;

        DataType data;
        
        [[nodiscard]] static FunctionResponsePart fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct TextData
    {
        std::string text;

        [[nodiscard]] static TextData fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };
    
    struct Blob
    {
        std::string mimeType;
        std::string data; // Base64

        
        [[nodiscard]] static Blob createFromPath(const std::string& filepath, const std::string& customMimeType = "");
        [[nodiscard]] static Blob fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct FunctionCall
    {
        std::optional<std::string> id;
        std::string name;
        nlohmann::json args;

        [[nodiscard]] static FunctionCall fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };
    
    FrenumClassInNamespace(GeminiCPP, Scheduling, uint8_t,
        SCHEDULING_UNSPECIFIED, // This value is unused.
        SILENT,                 // Only add the result to the conversation context, do not interrupt or trigger generation.
        WHEN_IDLE,              // Add the result to the conversation context, and prompt to generate output without interrupting ongoing generation.
        INTERRUPT               // Add the result to the conversation context, interrupt ongoing generation and prompt to generate output.
    )

    struct FunctionResponse
    {
        std::optional<std::string> id;
        std::string name;
        nlohmann::json responseContent;
        std::optional<std::vector<FunctionResponsePart>> parts;
        std::optional<bool> willContinue;
        std::optional<Scheduling> scheduling;
        
        [[nodiscard]] static FunctionResponse fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct FileData
    {
        std::optional<std::string> mimeType;
        std::string fileUri;

        [[nodiscard]] static FileData fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    FrenumClassInNamespace(GeminiCPP, Language, uint8_t,
        LANGUAGE_UNSPECIFIED,   // Unspecified language. This value should not be used.
        PYTHON                  // Python >= 3.10, numpy and simpy are available. The default language is Python.
    )

    struct ExecutableCode
    {
        Language language = Language::PYTHON;
        std::string code;

        [[nodiscard]] static ExecutableCode fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    FrenumClassInNamespace(GeminiCPP, Outcome, uint8_t,
        OUTCOME_UNSPECIFIED,        // Status not specified. This value should not be used.
        OUTCOME_OK,                 // Code execution completed successfully. 
        OUTCOME_FAILED,             // Code execution finished but with a failure. stderr should contain the reason.
        OUTCOME_DEADLINE_EXCEEDED   // Code execution ran for too long, and was cancelled. There may or may not be a partial output present.
    )
    
    struct CodeExecutionResult
    {
        Outcome outcome;
        std::optional<std::string> output;  // Console output

        [[nodiscard]] static CodeExecutionResult fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct VideoMetadata
    {
        std::optional<Duration> startOffset;
        std::optional<Duration> endOffset;
        std::optional<float> fps;

        [[nodiscard]] static VideoMetadata fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    struct Part
    {
        using DataType = std::variant<
            std::monostate, 
            TextData, 
            Blob, // inlineData
            FunctionCall, 
            FunctionResponse,
            FileData, 
            ExecutableCode,
            CodeExecutionResult 
        >;

        using MetadataType = std::variant<
            std::monostate,
            VideoMetadata
            >;
        
        DataType data;
        std::optional<bool> thought;
        std::optional<std::string> thoughtSignature; // Base64
        nlohmann::json partMetadata;
        MetadataType metadata;

        [[nodiscard]] bool isText() const;
        [[nodiscard]] bool isBlob() const; // Inline Base64
        [[nodiscard]] bool isFunctionCall() const;
        [[nodiscard]] bool isFunctionResponse() const;
        [[nodiscard]] bool isFileData() const; // URI Reference
        [[nodiscard]] bool isExecutableCode() const;
        [[nodiscard]] bool isCodeExecutionResult() const;

        [[nodiscard]] const TextData* getText() const;
        [[nodiscard]] const Blob* getBlob() const;
        [[nodiscard]] const FunctionCall* getFunctionCall() const;
        [[nodiscard]] const FunctionResponse* getFunctionResponse() const;
        [[nodiscard]] const FileData* getFileData() const;
        [[nodiscard]] const ExecutableCode* getExecutableCode() const;
        [[nodiscard]] const CodeExecutionResult* getCodeExecutionResult() const;

        [[nodiscard]] static Part Text(TextData t);
        [[nodiscard]] static Part Media(Blob blobData);
        [[nodiscard]] static Part Uri(FileData fileData);
        [[nodiscard]] static Part Call(FunctionCall call);
        [[nodiscard]] static Part Response(FunctionResponse resp);
        [[nodiscard]] static Part Code(ExecutableCode code);
        [[nodiscard]] static Part ExecutionResult(CodeExecutionResult result);
        
        [[nodiscard]] static Part fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };

    // Who's talking?
    FrenumClassInNamespace(GeminiCPP, Role, uint8_t,
        USER,
        MODEL,
        FUNCTION // I am not sure about this. Is it work?
    )

    struct Content
    {
        Role role = Role::USER; // Normally, this is optional, but we act like it's always there. I don't know why. I wanted to do it this way.
        std::vector<Part> parts;

        [[nodiscard]] static Content User();
        [[nodiscard]] static Content Model();
        [[nodiscard]] static Content Function();

        Content& text(const std::string& t);
        Content& image(const std::string& filepath);
        Content& file(const std::string& filepath);
        Content& fileUri(const std::string& uri, const std::string& mimeType);
        Content& media(const std::string& filepath, const std::string& mimeType = "");
        Content& functionResponse(FunctionResponse response);

        [[nodiscard]] nlohmann::json toJson() const;
        [[nodiscard]] static Content fromJson(const nlohmann::json& j);
    };

    FrenumClassInNamespace(GeminiCPP, ThinkingLevel, uint8_t,
        LOW,
        HIGH
    )
    
    struct ThinkingConfig
    {
        bool includeThoughts = true;
        std::optional<ThinkingLevel> thinkingLevel;
        std::optional<int> thinkingBudget;

        [[nodiscard]] nlohmann::json toJson() const;
    };

    // TODO: Update all the configs
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
        std::optional<ThinkingConfig> thinkingConfig;

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

    FrenumClassInNamespace(GeminiCPP, ContentPartModality, uint8_t,
        MODALITY_UNSPECIFIED,
        TEXT,
        IMAGE,
        VIDEO,
        AUDIO,
        DOCUMENT
    )

    FrenumClassInNamespace(GeminiCPP, HarmCategory, uint8_t,
        HARM_CATEGORY_UNSPECIFIED, // Default, not used.

        // Text Categories
        HARM_CATEGORY_HARASSMENT, // Content that contains harassment, threats or bullying.
        HARM_CATEGORY_HATE_SPEECH, // Content that promotes violence or hatred.
        HARM_CATEGORY_SEXUALLY_EXPLICIT, // Sexual content.
        HARM_CATEGORY_DANGEROUS_CONTENT, // Content that promotes dangerous activities.

        // PaLM
        HARM_CATEGORY_DEROGATORY, // Negative or harmful comments targeting identity and/or protected attribute.
        HARM_CATEGORY_TOXICITY, // Content that is rude, disrespectful, or profane.
        HARM_CATEGORY_VIOLENCE, // Describes scenarios depicting violence against an individual or group, or general descriptions of gore.
        HARM_CATEGORY_SEXUAL, // Contains references to sexual acts or other lewd content.
        HARM_CATEGORY_MEDICAL, // Promotes unchecked medical advice.
        HARM_CATEGORY_DANGEROUS, // Dangerous content that promotes, facilitates, or encourages harmful acts.

        // Image Categories
        HARM_CATEGORY_IMAGE_HATE,
        HARM_CATEGORY_IMAGE_DANGEROUS_CONTENT,
        HARM_CATEGORY_IMAGE_HARASSMENT,
        HARM_CATEGORY_IMAGE_SEXUALLY_EXPLICIT,

        // Special Categories
        HARM_CATEGORY_JAILBREAK // Prompts that attempt to bypass security filters.
    )
    
    FrenumClassInNamespace(GeminiCPP, HarmBlockThreshold, uint8_t,
        HARM_BLOCK_THRESHOLD_UNSPECIFIED, // Unspecified.
        BLOCK_LOW_AND_ABOVE,     // Block low risk and above (Very Strict).
        BLOCK_MEDIUM_AND_ABOVE,  // Block medium risk and above (Default).
        BLOCK_ONLY_HIGH,         // Only block high risk.
        BLOCK_NONE,              // Don't block anything (Risky).
        OFF                      // Turn off the filter completely.
    )

    FrenumClassInNamespace(GeminiCPP, HarmProbability, uint8_t,
        HARM_PROBABILITY_UNSPECIFIED,   // Probability is unspecified.
        NEGLIGIBLE,                     // Content has a negligible chance of being unsafe.
        LOW,                            // Content has a low chance of being unsafe.
        MEDIUM,                         // Content has a medium chance of being unsafe.
        HIGH                            // Content has a high chance of being unsafe.
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

    FrenumClassInNamespace(GeminiCPP, BlockReason, uint8_t,
        BLOCK_REASON_UNSPECIFIED,// Default value. This value is not used.
        SAFETY,                 // The request was blocked for security reasons. Examine the safetyRatings icon to understand which security category is blocking it.
        OTHER,                  // The request was blocked for unknown reasons.
        BLOCKLIST,              // The request was blocked due to terms included in the terminology blocklist.
        PROHIBITED_CONTENT,     // The request was blocked due to prohibited content.
        IMAGE_SAFETY            // Candidates were blocked due to unsafe image creation content.
    )

    FrenumClassInNamespace(GeminiCPP, FinishReason, uint8_t,
        FINISH_REASON_UNSPECIFIED,// Default.
        STOP,                   // Natural ending or stopping sequence.
        MAX_TOKENS,             // The maximum token limit has been reached.
        SAFETY,                 // Stopped due to security breach.
        RECITATION,             // Potential for copyright/quotation infringement.
        LANGUAGE,               // Answer candidate content was flagged for using unsupported language.
        OTHER,                  // Other reasons.
        BLOCKLIST,              // It stopped because it contained banned words.
        PROHIBITED_CONTENT,     // Potential for prohibited content.
        SPII,                   // It stopped because it contained Sensitive Personal Data (SPII).
        MALFORMED_FUNCTION_CALL,// The function call generated by the model is invalid.
        IMAGE_SAFETY,           // The token creation process was stopped because there were security breaches in the images produced.
        IMAGE_PROHIBITED_CONTENT,// Image creation was stopped because the created images contained other prohibited content.
        IMAGE_OTHER,            // Image generation has been halted due to various other issues.
        NO_IMAGE,               // The model was expected to generate an image, but no image was generated.
        IMAGE_RECITATION,       // Image production was stopped due to reading.
        UNEXPECTED_TOOL_CALL,   // The model generated a tool call, but no tools were activated in the request.
        TOO_MANY_TOOL_CALLS,    // The system exited execution because the model called too many tools in succession.
        MISSING_THOUGHT_SIGNATURE,// The request is missing at least one thought signature.

        // The special case I added (it does not come from the API, it is generated by the Client)
        PROMPT_BLOCKED
    )

    struct FinishReasonHelper
    {
        [[nodiscard]] static FinishReason fromString(const std::string& reason);
        [[nodiscard]] static std::string toString(FinishReason reason);
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
        std::optional<Duration> ttl;

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

    struct RequestConfig
    {
        std::vector<Content> contents;
        std::optional<std::vector<Tool>> tools;
        std::optional<ToolConfig> config;
        std::optional<std::vector<SafetySetting>> safetySettings;
        std::optional<Content> systemInstruction;
        std::optional<GenerationConfig> generationConfig;
        std::optional<std::string> cachedContent;
    };
} // namespace GeminiCPP
#endif // GEMINI_TYPES_H