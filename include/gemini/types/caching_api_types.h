#pragma once

#ifndef GEMINI_CACHING_API_TYPES_H
#define GEMINI_CACHING_API_TYPES_H

#include <optional>
#include <string>
#include <vector>
#include <variant>

#include "../duration.h"
#include "../types_base.h"

#include "nlohmann/json.hpp"
#include "frenum.h"

namespace GeminiCPP
{
    /**
     * @brief Scheduling options for function calls.
     */
    FrenumClassInNamespace(GeminiCPP, Scheduling, uint8_t,
        SCHEDULING_UNSPECIFIED, // This value is unused.
        SILENT,                 // Only add the result to the conversation context, do not interrupt or trigger generation.
        WHEN_IDLE,              // Add the result to the conversation context, and prompt to generate output without interrupting ongoing generation.
        INTERRUPT               // Add the result to the conversation context, interrupt ongoing generation and prompt to generate output.
    )

    /**
     * @brief Supported programming languages for code execution.
     */
    FrenumClassInNamespace(GeminiCPP, Language, uint8_t,
        LANGUAGE_UNSPECIFIED,   // Unspecified language. This value should not be used.
        PYTHON                  // Python >= 3.10, numpy and simpy are available. The default language is Python.
    )

    /**
     * @brief Outcome of a code execution.
     */
    FrenumClassInNamespace(GeminiCPP, Outcome, uint8_t,
        OUTCOME_UNSPECIFIED,        // Status not specified. This value should not be used.
        OUTCOME_OK,                 // Code execution completed successfully. 
        OUTCOME_FAILED,             // Code execution finished but with a failure. stderr should contain the reason.
        OUTCOME_DEADLINE_EXCEEDED   // Code execution ran for too long, and was cancelled. There may or may not be a partial output present.
    )

    /**
     * @brief Role of the message sender.
     */
    FrenumClassInNamespace(GeminiCPP, Role, uint8_t,
        USER,
        MODEL,
        FUNCTION // I am not sure about this. Is it work?
    )

    /**
     * @brief Configuration for how the model should use provided functions.
     */
    FrenumClassInNamespace(GeminiCPP, FunctionCallingMode, uint8_t,
        MODE_UNSPECIFIED, // Unspecified function calling mode. This value should not be used.
        AUTO, // Default model behavior, model decides to predict either a function call or a natural language response.
        ANY, // Model is constrained to always predicting a function call only. If "allowedFunctionNames" are set, the predicted function call will be limited to any one of "allowedFunctionNames", else the predicted function call will be any one of the provided "functionDeclarations".
        NONE, // Model will not predict any function call. Model behavior is same as when not passing any function declarations.
        VALIDATED // Model decides to predict either a function call or a natural language response, but will validate function calls with constrained decoding. If "allowedFunctionNames" are set, the predicted function call will be limited to any one of "allowedFunctionNames", else the predicted function call will be any one of the provided "functionDeclarations".
    )

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

    struct ExecutableCode
    {
        Language language = Language::PYTHON;
        std::string code;

        [[nodiscard]] static ExecutableCode fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };
    
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

    /**
     * @brief Represents a part of a message content.
     * * Can be text, binary blob (image/audio), function call, function response, etc.
     * * This class uses std::variant to hold one of many possible data types.
     */
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

    /**
     * @brief Represents a single turn in a conversation.
     * * Consists of a role (User/Model) and a list of parts (Text/Image/Function).
     */
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

    /**
     * @brief A collection of tools (functions, search, etc.) provided to the model.
     */
    struct Tool
    {
        std::vector<FunctionDeclaration> functionDeclarations;
        std::optional<GoogleSearch> googleSearch;
        std::optional<CodeExecution> codeExecution;
        std::optional<GoogleMaps> googleMaps;

        [[nodiscard]] static Tool fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const;
    };
    
    struct CachedContentUsageMetadata
    {
        int tokenCount = 0;
    };

    /**
     * @brief Represents cached content on the server.
     * * Caching allows reusing large contexts (like books or codebases) across multiple requests
     * without re-uploading or re-processing them, saving tokens and time.
     */
    struct CachedContent
    {
        std::string name; // ID: "cachedContents/..."
        std::string model;
        std::string displayName;
        std::string createTime;
        std::string updateTime;
        std::string expireTime;
        std::optional<Duration> ttl; ///< Time-to-live.

        CachedContentUsageMetadata usage;

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
}

#endif // GEMINI_CACHING_API_TYPES_H