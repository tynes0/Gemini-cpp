#include "gemini/client.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "gemini/logger.h"
#include "gemini/model.h"
#include "gemini/http_status.h"
#include "internal/payload_builder.h"

namespace GeminiCPP
{
    Client::Client(std::string api_key) : api_key_(std::move(api_key)) {}

    RequestBuilder Client::request()
    {
        return RequestBuilder(this);
    }

    ChatSession Client::startChat(Model model, std::string systemInstruction)
    {
        return { this, model, std::move(systemInstruction) };
    }
    
    GenerationResult Client::generateContent(const std::string& prompt, std::string_view model_id)
    {
        std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + std::string(model_id) + ":generateContent";

        nlohmann::json payload = {
            {"contents", {{
                {"parts", {{{"text", prompt}}}}
            }}}
        };

        return submitRequest(url, payload);
    }

    GenerationResult Client::generateContent(const std::string& prompt, Model model)
    {
        return generateContent(prompt, modelStringRepresentation(model));
    }

    GenerationResult Client::streamGenerateContent(const std::string& prompt, const StreamCallback& callback, std::string_view model_id)
    {
        return GenerationResult();
    }

    GenerationResult Client::streamGenerateContent(const std::string& prompt, const StreamCallback& callback, Model model)
    {
        return streamFromBuilder(model, "", {Part::Text(prompt)}, {}, {}, callback);
    }

    std::optional<ModelInfo> Client::getModelInfo(std::string_view model_id)
    {
        std::string modelStr(model_id);
        if (modelStr.find("models/") == std::string::npos)
        {
            modelStr = "models/" + modelStr;
        }

        std::string url = "https://generativelanguage.googleapis.com/v1beta/" + modelStr;

        cpr::Response r = cpr::Get(
            cpr::Url{url},
            cpr::Header{
                {"x-goog-api-key", api_key_}
            },
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            GEMINI_ERROR("Model Info Error [{}]: {}", r.status_code, r.text);
            return std::nullopt;
        }

        try
        {
            auto json = nlohmann::json::parse(r.text);
            return ModelInfo::fromJson(json);
        }
        catch (const std::exception& e)
        {
            GEMINI_ERROR("Model Info Parse Error: {}", e.what());
            return std::nullopt;
        }
    }

    std::optional<ModelInfo> Client::getModelInfo(Model model)
    {
        return getModelInfo(modelStringRepresentation(model));
    }

    GenerationResult Client::submitRequest(const std::string& url, const nlohmann::json& payload)
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
                    errorMsg = jsonErr["error"]["message"].get<std::string>();
            }
            catch(...)
            {
                GEMINI_ERROR("Error message parsing failed!");
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
                 
                return GenerationResult::Success(content, r.status_code, inTok, outTok, reason);
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
        
        // PayloadBuilder ile tek satırda JSON oluştur!
        nlohmann::json payload = Internal::PayloadBuilder::build(
            {userContent},
            sys_instr, 
            config, 
            safetySettings, 
            tools
        );

        std::string url = "https://generativelanguage.googleapis.com/v1beta/models/"
        + std::string(modelStringRepresentation(model)) + ":generateContent";

        return submitRequest(url, payload);
    }

    GenerationResult Client::submitStreamRequest(const std::string& url, const nlohmann::json& payload, const StreamCallback& callback)
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
                {
                    break;
                }

                size_t jsonStart = startPos + 5;
                std::string jsonStr = buffer.substr(jsonStart, endPos - jsonStart);

                buffer.erase(0, endPos + delimiterLen);

                if (!jsonStr.empty() && jsonStr.front() == ' ') 
                    jsonStr.erase(0, 1);

                if (jsonStr.empty()) continue;

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
                    GEMINI_ERROR("Stream JSON Parse Error: {} \nWrong JSON: {}", e.what(), jsonStr);
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
            try {
                auto jErr = nlohmann::json::parse(r.text);
                if (jErr.contains("error")) 
                    errDetails = jErr["error"]["message"].get<std::string>();
            } catch(...) {}
            
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

        std::string url = "https://generativelanguage.googleapis.com/v1beta/models/"
            + std::string(modelStringRepresentation(model)) + ":streamGenerateContent?alt=sse"; 
        
        return submitStreamRequest(url, payload, callback);
    }
}
