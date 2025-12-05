#pragma once

#ifndef GEMINI_CLIENT_H
#define GEMINI_CLIENT_H

#include <string>
#include <future>
#include <map>

#include "generation_method.h"
#include "response.h"
#include "url.h"
#include "support.h"
#include "utils.h"
#include "types/generating_content_api_types.h"

// API Modules
#include "apis/files.h"
#include "apis/models.h"
#include "apis/tokens.h"

namespace GeminiCPP
{
    class RequestBuilder; 
    class ChatSession;

    /**
     * @brief The main client class for interacting with the Google Gemini API.
     * * This class acts as the central entry point for all API operations. It manages the API key,
     * network configurations, and provides access to specific API modules (Files, Models, Tokens).
     * It also handles the core content generation methods.
     */
    class Client
    {
    public:
        /**
         * @brief Constructs a new Client instance.
         * * @param api_key The Google Gemini API key used for authentication.
         */
        explicit Client(std::string api_key);
        
        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;
        Client(Client&&) = default;
        Client& operator=(Client&&) = default;
        ~Client() = default;

        // --- API MODULES ---
        
        /**
         * @brief Module for File API operations (upload, list, delete, get).
         */
        Files files;

        /**
         * @brief Module for Model API operations (list models, get model info).
         */
        Models models;

        /**
         * @brief Module for Token API operations (count tokens).
         */
        Tokens tokens;

        // --- CORE GENERATION METHODS ---

        /**
         * @brief Generates content using the specified model and a fully constructed request body.
         * * This is a low-level method that allows full control over the request payload.
         * * @param model The model identifier (e.g., "models/gemini-2.5-flash").
         * @param request The structured request body containing contents, tools, config, etc.
         * @return GenerationResult containing the generated content or error details.
         */
        [[nodiscard]] GenerationResult generateContent(const std::string& model, const GenerateContentRequestBody& request);

        /**
         * @brief Generates content in a streaming fashion.
         * * The callback function is invoked repeatedly as chunks of generated text arrive.
         * * @param model The model identifier.
         * @param request The structured request body.
         * @param callback A function to handle incoming string chunks.
         * @return GenerationResult containing the final accumulated content and usage metadata.
         */
        [[nodiscard]] GenerationResult streamGenerateContent(const std::string& model, const StreamGenerateContentRequestBody& request, const StreamCallback& callback);

        // --- CONVENIENCE OVERLOADS ---

        /**
         * @brief Helper method to generate content from a simple text prompt.
         * * @param prompt The user's input text.
         * @param model The model to use (default: gemini-2.0-flash).
         * @return GenerationResult containing the response.
         */
        [[nodiscard]] GenerationResult generateContent(const std::string& prompt, const std::string& model = "gemini-2.0-flash");

        /**
         * @brief Helper method for streaming content generation from a simple text prompt.
         * * @param prompt The user's input text.
         * @param callback Function to handle stream chunks.
         * @param model The model to use (default: gemini-2.0-flash).
         * @return GenerationResult containing the final response.
         */
        [[nodiscard]] GenerationResult streamGenerateContent(const std::string& prompt, const StreamCallback& callback, const std::string& model = "gemini-2.0-flash");

        // --- ASYNC METHODS ---

        /**
         * @brief Asynchronously generates content.
         * * @param model The model identifier.
         * @param request The request body.
         * @return std::future<GenerationResult> A future object to retrieve the result later.
         */
        [[nodiscard]] std::future<GenerationResult> generateContentAsync(std::string model, GenerateContentRequestBody request);

        /**
         * @brief Asynchronously generates content in a stream.
         * * @param model The model identifier.
         * @param request The request body.
         * @param callback Function to handle stream chunks.
         * @return std::future<GenerationResult> A future object for the final result.
         */
        [[nodiscard]] std::future<GenerationResult> streamGenerateContentAsync(std::string model, StreamGenerateContentRequestBody request, StreamCallback callback);

        // --- UTILITIES ---

        /**
         * @brief Verifies if the provided API key is valid by making a lightweight API call.
         * * @return Support::ApiValidationResult Result indicating validity and status code.
         */
        [[nodiscard]] Support::ApiValidationResult verifyApiKey();

        /**
         * @brief Sets the retry configuration for failed HTTP requests.
         * * @param config The new retry configuration (max retries, delay, etc.).
         */
        void setRetryConfig(const Support::RetryConfig& config);

        /**
         * @brief Gets the current retry configuration.
         * * @return const Support::RetryConfig& The current configuration.
         */
        [[nodiscard]] const Support::RetryConfig& getRetryConfig() const;

        /**
         * @brief Creates a RequestBuilder associated with this client.
         * * @return RequestBuilder A builder object for fluent request construction.
         */
        [[nodiscard]] RequestBuilder request();

        /**
         * @brief Starts a new multi-turn chat session.
         * * @param model The model to be used for the chat.
         * @param sessionName Optional name for the session.
         * @return ChatSession A new chat session object.
         */
        [[nodiscard]] ChatSession startChat(const std::string& model, std::string sessionName = "");

        // --- HTTP ENGINE (Public for Modules) ---
        
        /**
         * @brief Performs a generic HTTP POST request expecting a JSON response.
         * * @tparam ResponseType A type that implements the JsonSerializable concept.
         * @param url The target URL.
         * @param payload The JSON body to send.
         * @return Result<ResponseType> The parsed response or error.
         */
        template <JsonSerializable ResponseType>
        Result<ResponseType> post(const std::string& url, const nlohmann::json& payload);

        /**
         * @brief Performs a generic HTTP GET request.
         * * @tparam ResponseType A type that implements the JsonSerializable concept.
         * @param url The target URL.
         * @param params Query parameters to append to the URL.
         * @return Result<ResponseType> The parsed response or error.
         */
        template <JsonSerializable ResponseType>
        Result<ResponseType> get(const std::string& url, const std::map<std::string, std::string>& params = {});

        /**
         * @brief Performs a multipart HTTP POST request (typically for file uploads).
         * * @tparam ResponseType A type that implements the JsonSerializable concept.
         * @param endpoint The API endpoint (relative to upload base URL).
         * @param filePath Local path to the file to upload.
         * @param mimeType MIME type of the file.
         * @param metadata JSON metadata associated with the file.
         * @return Result<ResponseType> The parsed response or error.
         */
        template <JsonSerializable ResponseType>
        Result<ResponseType> postMultipart(const std::string& endpoint, const std::string& filePath, const std::string& mimeType, const nlohmann::json& metadata);

        /**
         * @brief Performs a generic HTTP DELETE request.
         * * @param url The target URL.
         * @return Result<bool> Success status.
         */
        Result<bool> deleteResource(const std::string& url);

    private:
        [[nodiscard]] GenerationResult submitRequest(const Url& url, const nlohmann::json& payload);
        [[nodiscard]] GenerationResult submitStreamRequest(const Url& url, const nlohmann::json& payload, const StreamCallback& callback);

        void postHelper(const Url& url, const nlohmann::json& payload, std::string& text, long& statusCode);
        void getHelper(const Url& url, const std::map<std::string, std::string>& params, std::string& text, long& statusCode);
        void multipartHelper(const Url& url, const std::string& filePath, const std::string& mimeType, const nlohmann::json& metadata, std::string& text, long& statusCode);

        std::string api_key_;
        Support::RetryConfig retryConfig_;
    };

    // --- GENERIC HTTP IMPLEMENTATION ---
    
    template <JsonSerializable ResponseType>
    Result<ResponseType> Client::post(const std::string& urlStr, const nlohmann::json& payload)
    {
        Url url(urlStr);
        long statusCode;
        std::string text;
        postHelper(url, payload, text, statusCode);

        if (!HttpMappedStatusCodeHelper::isSuccess(statusCode))
        {
            return Result<ResponseType>::Failure(Utils::parseErrorMessage(text), statusCode);
        }
        try
        {
            return Result<ResponseType>::Success(ResponseType::fromJson(nlohmann::json::parse(text)), statusCode);
        }
        catch (const std::exception& e)
        {
            using namespace std::string_literals;
            return Result<ResponseType>::Failure("Parse Error: "s + e.what(), statusCode);
        }
    }

    template <JsonSerializable ResponseType>
    Result<ResponseType> Client::get(const std::string& urlStr, const std::map<std::string, std::string>& params)
    {
        Url url(urlStr);
        long statusCode;
        std::string text;
        getHelper(url, params, text, statusCode);

        if (!HttpMappedStatusCodeHelper::isSuccess(statusCode))
        {
            return Result<ResponseType>::Failure(Utils::parseErrorMessage(text), statusCode);
        }
        try
        {
            return Result<ResponseType>::Success(ResponseType::fromJson(nlohmann::json::parse(text)), statusCode);
        }
        catch (const std::exception& e)
        {
            using namespace std::string_literals;
            return Result<ResponseType>::Failure("Parse Error: "s + e.what(), statusCode);
        }
    }

    template <JsonSerializable ResponseType>
    Result<ResponseType> Client::postMultipart(const std::string& endpoint, const std::string& filePath, const std::string& mimeType, const nlohmann::json& metadata)
    {
        Url url(endpoint, EndpointType::UPLOAD);
        
        long statusCode;
        std::string text;
        multipartHelper(url, filePath, mimeType, metadata, text, statusCode);

        if (!HttpMappedStatusCodeHelper::isSuccess(statusCode))
        {
            return Result<ResponseType>::Failure(Utils::parseErrorMessage(text), statusCode);
        }
        try
        {
            auto json = nlohmann::json::parse(text);
            return Result<ResponseType>::Success(ResponseType::fromJson(json), statusCode);
        }
        catch (const std::exception& e)
        {
            using namespace std::string_literals;
            return Result<ResponseType>::Failure("Parse Error: "s + e.what(), statusCode);
        }
    }
}

#endif // GEMINI_CLIENT_H