#include "gemini/client.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "gemini/logger.h"
#include "gemini/model.h"
#include "gemini/http_status.h"
#include "internal/payload_builder.h"

namespace GeminiCPP
{
    namespace 
    {
        template <typename ResponseStruct>
        Result<ResponseStruct> ResponseHelper(const Url& url, const std::string& api_key, const nlohmann::json& payload)
        {
            cpr::Response r = cpr::Post(
            cpr::Url{url},
            cpr::Header{
                {"Content-Type", "application/json"},
                {"x-goog-api-key", api_key}},
            cpr::Body{payload.dump()},
            cpr::VerifySsl(false)
        );

            if (!HttpStatusHelper::isSuccess(r.status_code))
            {
                std::string errorMsg = r.text;
                try
                {
                    auto jsonErr = nlohmann::json::parse(r.text);
                    if(jsonErr.contains("error"))
                        errorMsg = jsonErr["error"]["message"].get<std::string>();
                }
                catch(const std::exception& e)
                {
                    GEMINI_WARN("Error message parsing failed! ({})", e.what());
                }
    
                GEMINI_ERROR("Response Error [{}]: {}", r.status_code, errorMsg);
                
                return Result<ResponseStruct>::Failure(errorMsg, r.status_code);
            }
            try
            {
                return Result<ResponseStruct>::Success(ResponseStruct::fromJson(nlohmann::json::parse(r.text)), r.status_code);
            }
            catch (const std::exception& e)
            {
                GEMINI_ERROR("Response Parse Error: {}", e.what());
                return Result<ResponseStruct>::Failure(std::string("Parse Error: ") + e.what(), r.status_code);
            }
        }
    }
    
    Client::Client(std::string api_key) : api_key_(std::move(api_key)) {}

    RequestBuilder Client::request()
    {
        return RequestBuilder(this);
    }

    ChatSession Client::startChat(Model model, std::string sessionName, std::string systemInstruction)
    {
        return { this, model, std::move(sessionName), std::move(systemInstruction) };
    }

    ChatSession Client::startChat(std::string_view model, std::string sessionName, std::string systemInstruction)
    {
        return { this, model, std::move(sessionName), std::move(systemInstruction) };
    }

    GenerationResult Client::generateContent(const std::string& prompt, std::string_view model_id,
                                             const GenerationConfig& config, const std::vector<SafetySetting>& safetySettings)
    {
        nlohmann::json payload = Internal::PayloadBuilder::build(
            { Content::User().text(prompt) },
            "",                               // System Instruction
            config,                           // Config
            safetySettings                    // Safety
        );
        
        Url url(ModelName(model_id), ":generateContent");

        return submitRequest(url, payload);
    }

    GenerationResult Client::generateContent(const std::string& prompt, Model model,
        const GenerationConfig& config, const std::vector<SafetySetting>& safetySettings)
    {
        return generateContent(prompt, modelStringRepresentation(model), config, safetySettings);
    }

    GenerationResult Client::streamGenerateContent(const std::string& prompt, const StreamCallback& callback,
        std::string_view model_id, const GenerationConfig& config, const std::vector<SafetySetting>& safetySettings)
    {
        nlohmann::json payload = Internal::PayloadBuilder::build(
            { Content::User().text(prompt) },
            "",
            config,
            safetySettings
        );
        
        Url url(ModelName(model_id), ":streamGenerateContent");
        url.addQuery("alt", "sse");
        
        return submitStreamRequest(url, payload, callback);
    }

    GenerationResult Client::streamGenerateContent(const std::string& prompt, const StreamCallback& callback,
        Model model, const GenerationConfig& config, const std::vector<SafetySetting>& safetySettings)
    {
        return streamGenerateContent(prompt, callback, modelStringRepresentation(model), config, safetySettings);
    }

    Result<ModelInfo> Client::getModelInfo(Model model)
    {
        return getModelInfo(modelStringRepresentation(model));
    }

    Result<ModelInfo> Client::getModelInfo(std::string_view model_id)
    {
        Url url{ ModelName(model_id) };
        
        cpr::Response r = cpr::Get(
            cpr::Url{url},
            cpr::Header{
                {"x-goog-api-key", api_key_}
            },
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            std::string errorMsg = r.text;
            try
            {
                auto jsonErr = nlohmann::json::parse(r.text);
                if(jsonErr.contains("error"))
                    errorMsg = jsonErr["error"]["message"].value("message", r.text);
            }
            catch(const std::exception& e)
            {
                GEMINI_WARN("Error message parsing failed! ({})", e.what());
            }
            GEMINI_ERROR("Model Info Error [{}]: {}", r.status_code, errorMsg);
            
            return Result<ModelInfo>::Failure(errorMsg, r.status_code);
        }

        try
        {
            return Result<ModelInfo>::Success(ModelInfo::fromJson(nlohmann::json::parse(r.text)));
        }
        catch (const std::exception& e)
        {
            GEMINI_ERROR("Model Info Parse Error: {}", e.what());
            return Result<ModelInfo>::Failure(std::string("Parse Error: ") + e.what());
        }
    }

    Result<std::vector<ModelInfo>> Client::listModels()
    {
        Url url(std::string_view("models"));

        cpr::Response r = cpr::Get(
            cpr::Url{url},
            cpr::Header{
                {"x-goog-api-key", api_key_}
            },
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            std::string errorMsg = r.text;
            try
            {
                auto jsonErr = nlohmann::json::parse(r.text);
                if(jsonErr.contains("error"))
                    errorMsg = jsonErr["error"]["message"].value("message", r.text);
            }
            catch(const std::exception& e)
            {
                GEMINI_WARN("Error message parsing failed! ({})", e.what());
            }
            GEMINI_ERROR("ListModels Error [{}]: {}", r.status_code, errorMsg);
            return Result<std::vector<ModelInfo>>::Failure(errorMsg, r.status_code);
        }

        try
        {
            std::vector<ModelInfo> models;
            
            auto json = nlohmann::json::parse(r.text);
            if (json.contains("models") && json["models"].is_array())
            {
                for (const auto& item : json["models"])
                {
                    models.push_back(ModelInfo::fromJson(item));
                }
            }
            return Result<std::vector<ModelInfo>>::Success(models);
        }
        catch (const std::exception& e)
        {
            GEMINI_ERROR("ListModels Parse Error: {}", e.what());
            return Result<std::vector<ModelInfo>>::Failure(std::string("Parse Error: ") + e.what());
        }
    }

    ApiValidationResult Client::verifyApiKey()
    {
        // Lightest endpoint: List models
        Url url(std::string_view("models"));

        cpr::Response r = cpr::Get(
            cpr::Url{url},
            cpr::Header{
                {"x-goog-api-key", api_key_}
            },
            cpr::VerifySsl(false)
        );

        ApiValidationResult result;
        result.statusCode = static_cast<HttpStatusCode>(r.status_code);
        if (HttpStatusHelper::isSuccess(r.status_code))
        {
            result.isValid = true;
            result.message = "API Key is valid. Connection successful.";
            result.reason = "OK";
            result.statusCode = HttpStatusCode::OK;
            return result;
        }

        result.isValid = false;
        result.message = "Connection failed.";
        result.reason = "UNKNOWN";

        try
        {
            auto jsonErr = nlohmann::json::parse(r.text);
            if (jsonErr.contains("error"))
            {
                auto errorObj = jsonErr["error"];
                result.message = errorObj.value("message", "Unknown Error");
                result.reason = errorObj.value("status", "UNKNOWN_STATUS");
                
                if (errorObj.contains("details") && !errorObj["details"].empty())
                {
                    auto firstDetail = errorObj["details"][0];
                    if (firstDetail.contains("reason"))
                    {
                        result.reason = firstDetail["reason"].get<std::string>();
                    }
                }
            }
        }
        catch (...)
        {
            result.message = "Raw response: " + r.text;
        }
        
        return result;
    }

    Result<EmbedContentResponse> Client::embedContent(std::string_view model, const std::string& text,
        const EmbedConfig& config)
    {
        ModelName modelName(model);
        Url url(modelName, ":embedContent");

        nlohmann::json payload = Internal::PayloadBuilder::buildEmbedContent(
            Content::User().text(text), 
            modelName, 
            config
        );

        return ResponseHelper<EmbedContentResponse>(url, api_key_, payload);
    }

    Result<EmbedContentResponse> Client::embedContent(Model model, const std::string& text, const EmbedConfig& config)
    {
        return embedContent(std::string(modelStringRepresentation(model)), text, config);
    }

    Result<BatchEmbedContentsResponse> Client::batchEmbedContents(std::string_view model,
        const std::vector<std::string>& texts, const EmbedConfig& config)
    {
        ModelName modelName(model);
        Url url(modelName, ":batchEmbedContents");
        
        nlohmann::json payload = Internal::PayloadBuilder::buildBatchEmbedContent(texts, modelName, config);

        return ResponseHelper<BatchEmbedContentsResponse>(url, api_key_, payload);
    }

    Result<BatchEmbedContentsResponse> Client::batchEmbedContents(Model model,
        const std::vector<std::string>& texts, const EmbedConfig& config)
    {
        return batchEmbedContents(std::string(modelStringRepresentation(model)), texts, config);
    }

    Result<TokenCountResponse> Client::countTokens(std::string_view model, const std::vector<Content>& contents,
        const std::string& systemInstruction, const std::vector<Tool>& tools)
    {
        Url url(ModelName(model), ":countTokens");

        nlohmann::json payload = Internal::PayloadBuilder::build(
            contents, 
            systemInstruction, 
            {}, // Config (insignificant)
            {}, // Safety (insignificant)
            tools
        );

        return ResponseHelper<TokenCountResponse>(url, api_key_, payload);
    }

    Result<TokenCountResponse> Client::countTokens(Model model, const std::vector<Content>& contents,
        const std::string& systemInstruction, const std::vector<Tool>& tools)
    {
        return countTokens(modelStringRepresentation(model), contents, systemInstruction, tools);
    }

    Result<TokenCountResponse> Client::countTokens(std::string_view model, const std::string& text)
    {
        return countTokens(model, {Content::User().text(text)});
    }

    Result<TokenCountResponse> Client::countTokens(Model model, const std::string& text)
    {
        return countTokens(model, {Content::User().text(text)});
    }

    GenerationResult Client::submitRequest(const Url& url, const nlohmann::json& payload)
    {
        cpr::Response r = cpr::Post(
            cpr::Url{url},
            cpr::Header{
                {"Content-Type", "application/json"},
                {"x-goog-api-key", api_key_}
            },
            cpr::Body{payload.dump()}
        );
    
        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            std::string errorMsg = r.text;
            try
            {
                auto jsonErr = nlohmann::json::parse(r.text);
                if(jsonErr.contains("error"))
                    errorMsg = jsonErr["error"]["message"].value("message", r.text);
            }
            catch(const std::exception& e)
            {
                GEMINI_WARN("Error message parsing failed! ({})", e.what());
            }
    
            GEMINI_ERROR("API Error [{}]: {}", r.status_code, errorMsg);
            return GenerationResult::Failure(errorMsg, r.status_code);
        }
    
        try
        {
            auto json_response = nlohmann::json::parse(r.text);

            if (json_response.contains("promptFeedback"))
            {
                auto feedback = json_response["promptFeedback"];
                if (feedback.contains("blockReason"))
                {
                    std::string reason = feedback["blockReason"].get<std::string>();
                    if (reason == "SAFETY")
                    {
                        return GenerationResult::Failure("Prompt blocked by Safety Filter", frenum::value(HttpStatusCode::OK), FinishReason::PROMPT_BLOCKED);
                    }
                }
            }
            
            if (!json_response.contains("candidates") || json_response["candidates"].empty())
            {
                return GenerationResult::Failure("No candidates returned", frenum::value(HttpStatusCode::OK));
            }
    
            auto candidate = json_response["candidates"][0];

            FinishReason reason = FinishReason::FINISH_REASON_UNSPECIFIED;
            if (candidate.contains("finishReason"))
            {
                std::string reasonStr = candidate["finishReason"].get<std::string>();
                reason = FinishReasonHelper::fromString(reasonStr);
            }

            if (reason == FinishReason::SAFETY && !candidate.contains("content"))
            {
                return GenerationResult::Failure("Response blocked by Safety Filter", frenum::value(HttpStatusCode::OK), FinishReason::SAFETY);
            }
            
            if (candidate.contains("content"))
            {
                 Content content = Content::fromJson(candidate["content"]);

                int inTok = 0, outTok = 0;
                if (json_response.contains("usageMetadata"))
                {
                    auto usage = json_response["usageMetadata"];
                    inTok = usage.value("promptTokenCount", 0);
                    outTok = usage.value("candidatesTokenCount", 0);
                }

                std::optional<GroundingMetadata> grounding = std::nullopt;
                if (candidate.contains("groundingMetadata"))
                {
                    grounding = GroundingMetadata::fromJson(candidate["groundingMetadata"]);
                }
                // Sometimes the groundingMetadata may not be in the candidate but at the root level (depending on the API version).
                // But generally, it's in the candidate. If it's not in the root, we look at the candidate anyway.
                
                return GenerationResult::Success(content, r.status_code, inTok, outTok, reason, grounding);
            }
            
            return GenerationResult::Failure("Candidate has no content", frenum::value(HttpStatusCode::OK), reason);
        }
        catch (const std::exception& e)
        {
            GEMINI_ERROR("JSON Parse Error: {}", e.what());
            return GenerationResult::Failure("Response parse error", r.status_code);
        }
    }

    GenerationResult Client::generateFromBuilder(Model model, const std::string& sys_instr, const std::vector<Part>& parts,
        const GenerationConfig& config, const std::vector<SafetySetting>& safetySettings, const std::vector<Tool>& tools)
    {
        Content userContent = Content::User();
        userContent.parts = parts; 
        
        nlohmann::json payload = Internal::PayloadBuilder::build(
            {userContent},
            sys_instr, 
            config, 
            safetySettings, 
            tools
        );

        Url url(ModelName(modelStringRepresentation(model)), ":generateContent");

        return submitRequest(url, payload);
    }

    GenerationResult Client::submitStreamRequest(const Url& url, const nlohmann::json& payload, const StreamCallback& callback)
    {
        std::string fullTextAccumulator;
        std::string buffer;
        int inputTokens = 0;
        int outputTokens = 0;

        auto write_func = [&](std::string data, intptr_t userdata) -> bool
        {
            (void)userdata;
            buffer += data;

            while (true)
            {
                size_t startPos = buffer.find("data:");
                
                if (startPos == std::string::npos)
                {
                    if (buffer.size() > (static_cast<size_t>(1024) * 1024))
                        buffer.clear(); 
                    break; 
                }

                size_t endPos = buffer.find("\n\n", startPos);
                size_t delimiterLen = 2;

                size_t crlfPos = buffer.find("\r\n\r\n", startPos);
                if (crlfPos != std::string::npos && (endPos == std::string::npos || crlfPos < endPos))
                {
                    endPos = crlfPos;
                    delimiterLen = 4;
                }

                if (endPos == std::string::npos)
                    break;

                size_t jsonStart = startPos + 5;
                std::string jsonStr = buffer.substr(jsonStart, endPos - jsonStart);

                buffer.erase(0, endPos + delimiterLen);

                if (!jsonStr.empty() && jsonStr.front() == ' ') 
                    jsonStr.erase(0, 1);

                if (jsonStr.empty())
                    continue;

                try
                {
                    auto jsonChunk = nlohmann::json::parse(jsonStr);

                    if (jsonChunk.contains("candidates") && !jsonChunk["candidates"].empty())
                    {
                        auto candidate = jsonChunk["candidates"][0];
                        if (candidate.contains("content") && candidate["content"].contains("parts"))
                        {
                            auto parts = candidate["content"]["parts"];
                            if (!parts.empty() && parts[0].contains("text"))
                            {
                                std::string chunkText = parts[0]["text"].get<std::string>();
                                
                                if (callback)
                                    callback(chunkText);
                                
                                fullTextAccumulator += chunkText;
                            }
                        }
                    }
                    
                    if (jsonChunk.contains("usageMetadata"))
                    {
                        inputTokens = jsonChunk["usageMetadata"].value("promptTokenCount", 0);
                        outputTokens = jsonChunk["usageMetadata"].value("candidatesTokenCount", 0);
                    }

                }
                catch (const std::exception& e)
                {
                    GEMINI_WARN("Stream JSON Parse Error: {} \nWrong JSON: {}", e.what(), jsonStr);
                }
            }
            return true;
        };

        cpr::Response r = cpr::Post(
            cpr::Url{url},
            cpr::Header{
                {"Content-Type", "application/json"},
                {"x-goog-api-key", api_key_}
            },
            cpr::Body{payload.dump()},
            cpr::WriteCallback(write_func),
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            std::string errDetails = r.text;
            try
            {
                auto jErr = nlohmann::json::parse(r.text);
                if (jErr.contains("error")) 
                    errDetails = jErr["error"]["message"].value("message", r.text);
            }
            catch(const std::exception& e)
            {
                GEMINI_WARN("Stream API Error detail parsing failed. ({})", e.what());
            }
            
            GEMINI_ERROR("Stream API Error [{}]: {}", r.status_code, errDetails);
            return GenerationResult::Failure(errDetails, r.status_code);
        }

        Content finalContent = Content::Model().text(fullTextAccumulator);
        return GenerationResult::Success(finalContent, r.status_code, inputTokens, outputTokens);
    }

    GenerationResult Client::streamFromBuilder(Model model, const std::string& sys_instr,
        const std::vector<Part>& parts, const GenerationConfig& config,
        const std::vector<SafetySetting>& safetySettings, const StreamCallback& callback, const std::vector<Tool>& tools)
    {
        Content userContent = Content::User();
        userContent.parts = parts;

        nlohmann::json payload = Internal::PayloadBuilder::build(
            {userContent}, 
            sys_instr, 
            config, 
            safetySettings, 
            tools
        );

        Url url(ModelName(modelStringRepresentation(model)), ":streamGenerateContent");
        url.addQuery("alt", "sse");
        
        return submitStreamRequest(url, payload, callback);
    }
}
