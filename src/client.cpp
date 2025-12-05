#include "gemini/client.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <cmath>
#include <random>
#include <thread>

#include "gemini/logger.h"
#include "gemini/request_builder.h" 
#include "gemini/chat_session.h"
#include "gemini/http_mapped_status_code.h"
#include "gemini/utils.h"
#include "gemini/types/models_api_types.h"

using namespace std::string_literals;

namespace GeminiCPP
{
    namespace 
    {
        // Helper to calculate backoff delay
        int calculateWaitTime(const Support::RetryConfig& config, int attempt, const cpr::Response& r)
        {
            if (r.header.contains("Retry-After"))
            {
                try
                {
                    return std::stoi(r.header.at("Retry-After")) * 1000;
                }
                catch (const std::exception& e)
                {
                    GEMINI_WARN("Retry-After to int failed. ({})", e.what());
                }
            }

            int delay = config.initialDelayMs * static_cast<int>(std::pow(config.multiplier, attempt));
            delay = (std::min)(delay, config.maxDelayMs);

            if (config.enableJitter)
            {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 500);
                delay += dis(gen);
            }
            return delay;
        }
    }
    
    Client::Client(std::string api_key) 
        : files(this), models(this), tokens(this), api_key_(std::move(api_key)) 
    {}

    void Client::setRetryConfig(const Support::RetryConfig& config) { retryConfig_ = config; }
    const Support::RetryConfig& Client::getRetryConfig() const { return retryConfig_; }

    // --- CORE GENERATION ---

    GenerationResult Client::generateContent(const std::string& model, const GenerateContentRequestBody& request)
    {
        Url url(ResourceName::Model(model), GM_GENERATE_CONTENT);
        // Type-safe JSON conversion
        return submitRequest(url, request.toJson());
    }

    GenerationResult Client::streamGenerateContent(const std::string& model, const StreamGenerateContentRequestBody& request, const StreamCallback& callback)
    {
        Url url(ResourceName::Model(model), GM_STREAM_GENERATE_CONTENT);
        url.addQuery("alt", "sse");
        return submitStreamRequest(url, request.toJson(), callback);
    }

    // --- CONVENIENCE OVERLOADS ---

    GenerationResult Client::generateContent(const std::string& prompt, const std::string& model)
    {
        // Construct a simple request body using the new types
        GenerateContentRequestBody req;
        req.contents.push_back(Content::User().text(prompt));
        
        return generateContent(model, req);
    }

    GenerationResult Client::streamGenerateContent(const std::string& prompt, const StreamCallback& callback, const std::string& model)
    {
        StreamGenerateContentRequestBody req;
        req.contents.push_back(Content::User().text(prompt));

        return streamGenerateContent(model, req, callback);
    }

    // --- ASYNC WRAPPERS ---

    std::future<GenerationResult> Client::generateContentAsync(std::string model, GenerateContentRequestBody request)
    {
        return std::async(std::launch::async, [this, m = std::move(model), r = std::move(request)]() {
            return generateContent(m, r);
        });
    }

    std::future<GenerationResult> Client::streamGenerateContentAsync(std::string model, StreamGenerateContentRequestBody request, StreamCallback callback)
    {
        return std::async(std::launch::async, [this, m = std::move(model), r = std::move(request), cb = std::move(callback)]() {
            return streamGenerateContent(m, r, cb);
        });
    }

    // --- FACTORY METHODS ---

    RequestBuilder Client::request()
    {
        return RequestBuilder(this);
    }

    ChatSession Client::startChat(const std::string& model, std::string sessionName)
    {
        return {this, model, std::move(sessionName) };
    }

    // --- HTTP ENGINE & UTILS ---

    Support::ApiValidationResult Client::verifyApiKey()
    {
        // Simple call to list models to verify key
        auto res = get<ModelsGetResponseBody>("models"); // Generic get call
        
        Support::ApiValidationResult result;
        if (res.success)
        {
            result.isValid = true;
            result.message = "OK";
            result.statusCode = HttpMappedStatusCode::OK;
        } else
        {
            result.isValid = false;
            result.message = res.errorMessage;
            result.statusCode = res.statusCode;
        }
        return result;
    }

    GenerationResult Client::submitRequest(const Url& url, const nlohmann::json& payload)
    {
        int attempt = 0;
        while (true)
        {
            cpr::Response r = cpr::Post(
                cpr::Url{url.str()},
                cpr::Header{
                    {"Content-Type", "application/json"},
                    {"x-goog-api-key", api_key_}
                },
                cpr::Body{payload.dump()},
                cpr::VerifySsl(false)
            );

            if (HttpMappedStatusCodeHelper::isSuccess(r.status_code))
            {
                try
                {
                    auto json_response = nlohmann::json::parse(r.text);
                    auto responseBody = GenerateContentResponseBody::fromJson(json_response);

                    // 1. Prompt Feedback Check
                    if (responseBody.promptFeedback.blockReason.has_value() && responseBody.promptFeedback.blockReason == BlockReason::SAFETY) 
                    {
                        return GenerationResult::Failure("Prompt blocked by Safety Filter", static_cast<int>(r.status_code), FinishReason::PROMPT_BLOCKED);
                    }
                    
                    // 2. Candidate Check
                    if (responseBody.candidates.empty()) 
                    {
                        if (!responseBody.promptFeedback.safetyRatings.empty())
                        {
                             return GenerationResult::Failure("No candidates returned (Safety?)", static_cast<int>(r.status_code));
                        }
                        return GenerationResult::Failure("No candidates returned", static_cast<int>(r.status_code));
                    }

                    const auto& candidate = responseBody.candidates[0];
                    
                    // 3. Construct Result
                    // Grounding metadata might be attached to candidate
                    std::optional<GroundingMetadata> grounding = std::nullopt;
                    // Note: In some API versions groundingMetadata is inside candidate, in types we put it there.
                    grounding = candidate.groundingMetadata;

                    return GenerationResult::Success(
                        candidate.content,
                        static_cast<int>(r.status_code),
                        responseBody.usageMetadata.promptTokenCount,
                        responseBody.usageMetadata.candidatesTokenCount,
                        candidate.finishReason.value_or(FinishReason::FINISH_REASON_UNSPECIFIED),
                        grounding
                    );
                }
                catch (const std::exception& e)
                {
                    GEMINI_ERROR("SubmitRequest JSON Parse Error: {}", e.what());
                    return GenerationResult::Failure("JSON Parse Error: "s + e.what(), r.status_code);
                }
            }

            if (HttpMappedStatusCodeHelper::isRetryable(r.status_code) && attempt < retryConfig_.maxRetries)
            {
                int waitMs = calculateWaitTime(retryConfig_, attempt, r);
                GEMINI_WARN("API Retry [{}]: {}ms", r.status_code, waitMs);
                std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
                attempt++;
                continue;
            }

            return GenerationResult::Failure(Utils::parseErrorMessage(r.text), r.status_code);
        }
    }

    GenerationResult Client::submitStreamRequest(const Url& url, const nlohmann::json& payload, const StreamCallback& callback)
    {
        int attempt = 0;
        
        while (true)
        {
            std::string fullTextAccumulator;
            std::string buffer;
            int inputTokens = 0;
            int outputTokens = 0;
            bool dataReceived = false;
            FinishReason lastFinishReason = FinishReason::FINISH_REASON_UNSPECIFIED;

            auto write_func = [&](std::string data, intptr_t userdata) -> bool
            {
                (void)userdata;
                if (!data.empty()) dataReceived = true;
                
                buffer += data;

                while (true)
                {
                    size_t startPos = buffer.find("data:");
                    if (startPos == std::string::npos)
                    {
                        // (safety valve)
                        if (buffer.size() > (static_cast<size_t>(4096) * 10))
                            buffer.clear(); 
                        break; 
                    }

                    // SSE event boundary
                    size_t endPos = buffer.find("\n\n", startPos);
                    size_t delimiterLen = 2;

                    // Windows style CRLF check
                    size_t crlfPos = buffer.find("\r\n\r\n", startPos);
                    if (crlfPos != std::string::npos && (endPos == std::string::npos || crlfPos < endPos))
                    {
                        endPos = crlfPos;
                        delimiterLen = 4;
                    }

                    if (endPos == std::string::npos)
                        break; // Packet not completed

                    size_t jsonStart = startPos + 5; // "data:".length()
                    std::string jsonStr = buffer.substr(jsonStart, endPos - jsonStart);

                    buffer.erase(0, endPos + delimiterLen);

                    // Trim leading spaces
                    if (!jsonStr.empty() && jsonStr.front() == ' ')
                        jsonStr.erase(0, 1);
                    if (jsonStr.empty())
                        continue;

                    try
                    {
                        auto jsonChunk = nlohmann::json::parse(jsonStr);
                        auto responseChunk = StreamGenerateContentResponseBody::fromJson(jsonChunk);

                        if (!responseChunk.response.candidates.empty())
                        {
                            const auto& candidate = responseChunk.response.candidates[0];
                            
                            // Text extraction logic
                            for (const auto& part : candidate.content.parts)
                            {
                                if (part.isText())
                                {
                                    if (const auto* txtData = part.getText())
                                    {
                                        if (callback) callback(txtData->text);
                                        fullTextAccumulator += txtData->text;
                                    }
                                }
                            }
                            
                            if (candidate.finishReason.has_value())
                                lastFinishReason = candidate.finishReason.value();
                        }
                        
                        // Update tokens if present (usually in the last chunk)
                        if (responseChunk.response.usageMetadata.totalTokenCount > 0)
                        {
                            inputTokens = responseChunk.response.usageMetadata.promptTokenCount;
                            outputTokens = responseChunk.response.usageMetadata.candidatesTokenCount;
                        }
                    }
                    catch (const std::exception& e)
                    {
                        GEMINI_WARN("Stream JSON Parse Error: {}", e.what());
                    }
                }
                return true;
            };

            cpr::Response r = cpr::Post(
                cpr::Url{url.str()},
                cpr::Header{
                    {"Content-Type", "application/json"},
                    {"x-goog-api-key", api_key_}
                },
                cpr::Body{payload.dump()},
                cpr::WriteCallback(write_func),
                cpr::VerifySsl(false)
            );

            if (HttpMappedStatusCodeHelper::isSuccess(r.status_code))
            {
                Content finalContent = Content::Model().text(fullTextAccumulator);
                return GenerationResult::Success(finalContent, static_cast<int>(r.status_code), inputTokens, outputTokens, lastFinishReason);
            }

            if (HttpMappedStatusCodeHelper::isRetryable(r.status_code) && attempt < retryConfig_.maxRetries && !dataReceived)
            {
                int waitMs = calculateWaitTime(retryConfig_, attempt, r);
                GEMINI_WARN("Stream API Error [{}]. Retrying... ({}/{})", r.status_code, attempt + 1, retryConfig_.maxRetries);
                std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
                attempt++;
                continue;
            }

            std::string errDetails = Utils::parseErrorMessage(r.text);
            GEMINI_ERROR("Stream API Error [{}]: {}", r.status_code, errDetails);
            return GenerationResult::Failure(errDetails, r.status_code);
        }
    }

    void Client::postHelper(const Url& url, const nlohmann::json& payload, std::string& text, long& statusCode)
    {
        cpr::Response r = cpr::Post(
            cpr::Url{url.str()},
            cpr::Header{
                {"Content-Type", "application/json"},
                {"x-goog-api-key", api_key_}
            },
            cpr::Body{payload.dump()},
            cpr::VerifySsl(false)
        );

        text = r.text;
        statusCode = r.status_code;
    }

    void Client::getHelper(const Url& url, const std::map<std::string, std::string>& params, std::string& text, long& statusCode)
    {
        cpr::Parameters cprParams;
        for(const auto& [k, v] : params) cprParams.Add({k, v});

        cpr::Response r = cpr::Get(
            cpr::Url{url.str()},
            cpr::Header{{"x-goog-api-key", api_key_}},
            cprParams,
            cpr::VerifySsl(false)
        );

        text = r.text;
        statusCode = r.status_code;
    }

    void Client::multipartHelper(const Url& url, const std::string& filePath, const std::string& mimeType, const nlohmann::json& metadata, std::string& text, long& statusCode)
    {
        cpr::Response r = cpr::Post(
            cpr::Url{url.str()},
            cpr::Header{
                {"x-goog-api-key", api_key_},
                {"X-Goog-Upload-Protocol", "multipart"},
                {"X-Goog-Upload-Mime-Type", mimeType}
            },
            cpr::Multipart{
                {"metadata", metadata.dump(), "application/json"},
                {"file", cpr::File(filePath), mimeType}
            },
            cpr::VerifySsl(false)
        );
        text = r.text;
        statusCode = r.status_code;
    }

    Result<bool> Client::deleteResource(const std::string& urlStr)
    {
        Url url(urlStr);
        cpr::Response r = cpr::Delete(
            cpr::Url{url.str()},
            cpr::Header{{"x-goog-api-key", api_key_}},
            cpr::VerifySsl(false)
        );
        
        if (HttpMappedStatusCodeHelper::isSuccess(r.status_code))
            return Result<bool>::Success(true);
            
        return Result<bool>::Failure(Utils::parseErrorMessage(r.text), r.status_code);
    }
}