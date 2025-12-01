#pragma once

#ifndef GEMINI_CLIENT_H
#define GEMINI_CLIENT_H

#include <string>
#include <future>
#include <map>

#include "model.h"
#include "response.h"
#include "url.h"
#include "support.h"
#include "utils.h"
#include "types/generating_content_api_types.h"

namespace GeminiCPP
{
    // Forward declaration
    class RequestBuilder; 
    class ChatSession;

    class Client
    {
    public:
        explicit Client(std::string api_key);
        
        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;
        Client(Client&&) = default;
        Client& operator=(Client&&) = default;
        ~Client() = default;

        // --- CORE GENERATION METHODS ---
        [[nodiscard]] GenerationResult generateContent(const std::string& model, const GenerateContentRequestBody& request);
        [[nodiscard]] GenerationResult streamGenerateContent(const std::string& model, const StreamGenerateContentRequestBody& request, const StreamCallback& callback);

        // --- CONVENIENCE OVERLOADS ---
        [[nodiscard]] GenerationResult generateContent(const std::string& prompt, Model model = Model::GEMINI_2_5_FLASH);
        [[nodiscard]] GenerationResult streamGenerateContent(const std::string& prompt, const StreamCallback& callback, Model model = Model::GEMINI_2_0_FLASH);

        // --- ASYNC METHODS ---
        [[nodiscard]] std::future<GenerationResult> generateContentAsync(std::string model, GenerateContentRequestBody request);
        [[nodiscard]] std::future<GenerationResult> streamGenerateContentAsync(std::string model, StreamGenerateContentRequestBody request, StreamCallback callback);

        // --- UTILITIES ---
        [[nodiscard]] Support::ApiValidationResult verifyApiKey();

        void setRetryConfig(const Support::RetryConfig& config);
        [[nodiscard]] const Support::RetryConfig& getRetryConfig() const;

        [[nodiscard]] RequestBuilder request();
        [[nodiscard]] ChatSession startChat(Model model = Model::GEMINI_2_0_FLASH, std::string sessionName = "", std::string systemInstruction = "");

        // --- HTTP ENGINE ---
        // Template constraint: JsonSerializable
        template <JsonSerializable ResponseType>
        Result<ResponseType> post(const std::string& url, const nlohmann::json& payload);

        template <JsonSerializable ResponseType>
        Result<ResponseType> get(const std::string& url, const std::map<std::string, std::string>& params = {});

        Result<bool> deleteResource(const std::string& url);

    private:
        [[nodiscard]] GenerationResult submitRequest(const Url& url, const nlohmann::json& payload);
        [[nodiscard]] GenerationResult submitStreamRequest(const Url& url, const nlohmann::json& payload, const StreamCallback& callback);

        void postHelper(const Url& url, const nlohmann::json& payload, std::string& text, long& statusCode);
        void getHelper(const Url& url, const std::map<std::string, std::string>& params, std::string& text, long& statusCode);

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
}

#endif // GEMINI_CLIENT_H