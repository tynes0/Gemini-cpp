#include "gemini/client.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <cmath>
#include <random>

#include "gemini/logger.h"
#include "gemini/model.h"
#include "gemini/http_status.h"
#include "gemini/utils.h"
#include "internal/payload_builder.h"

using namespace std::string_literals;

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
                std::string errorMsg = Utils::parseErrorMessage(r.text);
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
                return Result<ResponseStruct>::Failure("Parse Error: "s + e.what(), r.status_code);
            }
        }

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
                    GEMINI_ERROR("Retry-After parsing failed: {}", e.what());
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

    GenerationResult Client::generateContent(const std::string& prompt, std::string_view model_id)
    {
        nlohmann::json payload = Internal::PayloadBuilder::build(
            { Content::User().text(prompt) }
        );
        
        Url url(ResourceName::Model(std::string(model_id)), GM_GENERATE_CONTENT);

        return submitRequest(url, payload);
    }

    GenerationResult Client::generateContent(const std::string& prompt, Model model)
    {
        return generateContent(prompt, ModelHelper::stringRepresentation(model));
    }

    GenerationResult Client::streamGenerateContent(const std::string& prompt, const StreamCallback& callback, std::string_view model_id)
    {
        nlohmann::json payload = Internal::PayloadBuilder::build(
            { Content::User().text(prompt) }
        );
        
        Url url(ResourceName::Model(std::string(model_id)), GM_STREAM_GENERATE_CONTENT);
        url.addQuery("alt", "sse");
        
        return submitStreamRequest(url, payload, callback);
    }

    GenerationResult Client::streamGenerateContent(const std::string& prompt, const StreamCallback& callback, Model model)
    {
        return streamGenerateContent(prompt, callback, ModelHelper::stringRepresentation(model));
    }

    Result<File> Client::uploadFile(const std::string& path, std::string displayName)
    {
        if (!std::filesystem::exists(path))
            return Result<File>::Failure("File not found: " + path);

        std::string mimeType = Utils::getMimeType(path);

        if (displayName.empty())
            displayName = std::filesystem::path(path).filename().string();

        Url url("files", EndpointType::UPLOAD);

        // The Upload API may sometimes request the API Key as a query parameter.
        // But we will add (x-goog-api-key) to the header; generally, both will work.
        // url.addQuery("key", api_key_);

        nlohmann::json metadata = {
            {"file", {
                    {"display_name", displayName}
            }}
        };

        cpr::Response r = cpr::Post(
            cpr::Url{url},
            cpr::Header{
                    {"x-goog-api-key", api_key_}, // Auth Header
                    {"X-Goog-Upload-Protocol", "multipart"},
                    {"X-Goog-Upload-Mime-Type", mimeType}
            },
            cpr::Multipart{
                {"metadata", metadata.dump(), "application/json"},
                {"file", cpr::File(path), mimeType}
            },
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            std::string err = Utils::parseErrorMessage(r.text);
            GEMINI_ERROR("Upload Failed [{}]: {}", r.status_code, err);
            return Result<File>::Failure(err, r.status_code);
        }

        try
        {
            auto json = nlohmann::json::parse(r.text);
            if (json.contains("file"))
                return Result<File>::Success(File::fromJson(json["file"]));
            
            return Result<File>::Success(File::fromJson(json));
        }
        catch (const std::exception& e)
        {
            return Result<File>::Failure("Parse Error: "s + e.what());
        }
    }

    Result<File> Client::getFile(const std::string& name)
    {
        Url url(ResourceName::File(name));

        cpr::Response r = cpr::Get(
            cpr::Url{url},
            cpr::Header{{"x-goog-api-key", api_key_}},
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            return Result<File>::Failure(Utils::parseErrorMessage(r.text), r.status_code);
        }

        try
        {
            return Result<File>::Success(File::fromJson(nlohmann::json::parse(r.text)));
        }
        catch (const std::exception& e)
        {
            return Result<File>::Failure("Parse Error: "s + e.what());
        }
    }

    Result<bool> Client::deleteFile(const std::string& name)
    {
        Url url(ResourceName::File(name));

        cpr::Response r = cpr::Delete(
            cpr::Url{url},
            cpr::Header{{"x-goog-api-key", api_key_}},
            cpr::VerifySsl(false)
        );

        if (HttpStatusHelper::isSuccess(r.status_code))
        {
            return Result<bool>::Success(true);
        }
        
        return Result<bool>::Failure(Utils::parseErrorMessage(r.text), r.status_code);
    }
    
    Result<ListFilesResponse> Client::listFiles(int pageSize, const std::string& pageToken)
    {
        Url url(std::string_view("files"));
        
        cpr::Parameters params;
        params.Add({"pageSize", std::to_string(pageSize)});
        if(!pageToken.empty())
            params.Add({"pageToken", pageToken});

        cpr::Response r = cpr::Get(
            cpr::Url{url},
            cpr::Header{{"x-goog-api-key", api_key_}},
            params, // Query Params
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            return Result<ListFilesResponse>::Failure(Utils::parseErrorMessage(r.text), r.status_code);
        }

        try
        {
            return Result<ListFilesResponse>::Success(ListFilesResponse::fromJson(nlohmann::json::parse(r.text)));
        }
        catch (const std::exception& e)
        {
            return Result<ListFilesResponse>::Failure("Parse Error: "s + e.what());
        }
    }

    Result<ModelInfo> Client::getModelInfo(Model model)
    {
        return getModelInfo(ModelHelper::stringRepresentation(model));
    }

    Result<ModelInfo> Client::getModelInfo(std::string_view model_id)
    {
        Url url{ ResourceName::Model(std::string(model_id)) };
        
        cpr::Response r = cpr::Get(
            cpr::Url{url},
            cpr::Header{
                {"x-goog-api-key", api_key_}
            },
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            std::string errorMsg = Utils::parseErrorMessage(r.text);
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
            return Result<ModelInfo>::Failure("Parse Error: "s + e.what());
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
            std::string errorMsg = Utils::parseErrorMessage(r.text);
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
            return Result<std::vector<ModelInfo>>::Failure("Parse Error: "s + e.what());
        }
    }

    Support::ApiValidationResult Client::verifyApiKey()
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

        Support::ApiValidationResult result;
        result.statusCode = static_cast<HttpMappedStatusCode>(r.status_code);
        if (HttpStatusHelper::isSuccess(r.status_code))
        {
            result.isValid = true;
            result.message = "API Key is valid. Connection successful.";
            result.reason = "OK";
            result.statusCode = HttpMappedStatusCode::OK;
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
        const EmbedRequestBody& config)
    {
        ResourceName modelName = ResourceName::Model(std::string(model));
        Url url(modelName, GM_EMBED_CONTENT);

        nlohmann::json payload = Internal::PayloadBuilder::buildEmbedContent(
            Content::User().text(text), 
            modelName, 
            config
        );

        return ResponseHelper<EmbedContentResponse>(url, api_key_, payload);
    }

    Result<EmbedContentResponse> Client::embedContent(Model model, const std::string& text, const EmbedRequestBody& config)
    {
        return embedContent(std::string(ModelHelper::stringRepresentation(model)), text, config);
    }

    Result<BatchEmbedContentsResponse> Client::batchEmbedContents(std::string_view model,
        const std::vector<std::string>& texts, const EmbedRequestBody& config)
    {
        ResourceName modelName = ResourceName::Model(std::string(model));
        Url url(modelName, GM_BATCH_EMBED_CONTENTS);
        
        nlohmann::json payload = Internal::PayloadBuilder::buildBatchEmbedContent(texts, modelName, config);

        return ResponseHelper<BatchEmbedContentsResponse>(url, api_key_, payload);
    }

    Result<BatchEmbedContentsResponse> Client::batchEmbedContents(Model model,
        const std::vector<std::string>& texts, const EmbedRequestBody& config)
    {
        return batchEmbedContents(std::string(ModelHelper::stringRepresentation(model)), texts, config);
    }

    Result<CountTokensResponseBody> Client::countTokens(std::string_view model, const std::vector<Content>& contents,
        const std::string& systemInstruction, const std::vector<Tool>& tools)
    {
        Url url(ResourceName::Model(std::string(model)), GM_COUNT_TOKENS);

        nlohmann::json payload = Internal::PayloadBuilder::build(
            contents, 
            systemInstruction,
            "",
            {}, // Config (insignificant)
            {}, // Safety (insignificant)
            tools
        );

        return ResponseHelper<CountTokensResponseBody>(url, api_key_, payload);
    }

    Result<CountTokensResponseBody> Client::countTokens(Model model, const std::vector<Content>& contents,
        const std::string& systemInstruction, const std::vector<Tool>& tools)
    {
        return countTokens(ModelHelper::stringRepresentation(model), contents, systemInstruction, tools);
    }

    Result<CountTokensResponseBody> Client::countTokens(std::string_view model, const std::string& text)
    {
        return countTokens(model, {Content::User().text(text)});
    }

    Result<CountTokensResponseBody> Client::countTokens(Model model, const std::string& text)
    {
        return countTokens(model, {Content::User().text(text)});
    }

    Result<CachedContent> Client::createCachedContent(const CachedContent& contentConfig)
    {
        Url url(std::string_view("cachedContents"));
        cpr::Response r = cpr::Post(
            cpr::Url{url.str()},
            cpr::Header{
                {"Content-Type", "application/json"},
                {"x-goog-api-key", api_key_}
            },
            cpr::Body{contentConfig.toJson().dump()},
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            std::string err = Utils::parseErrorMessage(r.text);
            GEMINI_ERROR("CreateCache Error [{}]: {}", r.status_code, err);
            return Result<CachedContent>::Failure(err, r.status_code);
        }

        try
        {
            return Result<CachedContent>::Success(CachedContent::fromJson(nlohmann::json::parse(r.text)));
        }
        catch (const std::exception& e)
        {
            return Result<CachedContent>::Failure("Parse Error: "s + e.what());
        }
    }

    Result<CachedContent> Client::getCachedContent(const std::string& name)
    {
        Url url(ResourceName::CachedContent(name));

        cpr::Response r = cpr::Get(
            cpr::Url{url.str()},
            cpr::Header{{"x-goog-api-key", api_key_}},
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            return Result<CachedContent>::Failure(Utils::parseErrorMessage(r.text), r.status_code);
        }
        
        try
        {
            return Result<CachedContent>::Success(CachedContent::fromJson(nlohmann::json::parse(r.text)));
        }
        catch (const std::exception& e)
        {
            return Result<CachedContent>::Failure("Parse Error: "s + e.what());
        }
    }

    Result<ListCachedContentsResponse> Client::listCachedContents(int pageSize, const std::string& pageToken)
    {
        Url url(std::string_view("cachedContents"));
        cpr::Parameters params;
        params.Add({"pageSize", std::to_string(pageSize)});
        if(!pageToken.empty()) params.Add({"pageToken", pageToken});

        cpr::Response r = cpr::Get(
            cpr::Url{url.str()},
            cpr::Header{{"x-goog-api-key", api_key_}},
            params,
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            return Result<ListCachedContentsResponse>::Failure(Utils::parseErrorMessage(r.text), r.status_code);
        }
        try
        {
            return Result<ListCachedContentsResponse>::Success(ListCachedContentsResponse::fromJson(nlohmann::json::parse(r.text)));
        }
        catch (const std::exception& e)
        {
            return Result<ListCachedContentsResponse>::Failure("Parse Error: "s + e.what());
        }
    }

    Result<bool> Client::deleteCachedContent(const std::string& name)
    {
        Url url(ResourceName::Raw(name));
        cpr::Response r = cpr::Delete(
            cpr::Url{url.str()},
            cpr::Header{{"x-goog-api-key", api_key_}},
            cpr::VerifySsl(false)
        );

        if (HttpStatusHelper::isSuccess(r.status_code))
            return Result<bool>::Success(true);
        
        return Result<bool>::Failure(Utils::parseErrorMessage(r.text), r.status_code);
    }

    Result<CachedContent> Client::updateCachedContent(const std::string& name, const std::string& ttl)
    {
        Url url(ResourceName::Raw(name));
        nlohmann::json payload = { {"ttl", ttl} };

        cpr::Response r = cpr::Patch(
            cpr::Url{url.str()},
            cpr::Header{
                {"Content-Type", "application/json"},
                {"x-goog-api-key", api_key_}
            },
            cpr::Body{payload.dump()},
            cpr::VerifySsl(false)
        );

        if (!HttpStatusHelper::isSuccess(r.status_code))
        {
            return Result<CachedContent>::Failure(Utils::parseErrorMessage(r.text), r.status_code);
        }
        try
        {
            return Result<CachedContent>::Success(CachedContent::fromJson(nlohmann::json::parse(r.text)));
        }
        catch (const std::exception& e)
        {
            return Result<CachedContent>::Failure("Parse Error: "s + e.what());
        }
    }

    std::future<GenerationResult> Client::generateContentAsync(std::string prompt, std::string_view model_id)
    {
        std::string modelIdStr(model_id);
        return std::async(std::launch::async, [
            this,
            modelIdStr = std::move(modelIdStr),
            prompt = std::move(prompt)]() {
            return generateContent(prompt, modelIdStr);
        });
    }

    std::future<GenerationResult> Client::generateContentAsync(std::string prompt, Model model)
    {
        return std::async(std::launch::async, [
            this,
            model,
            prompt = std::move(prompt)]() {
            return generateContent(prompt, model);
        });
    }

    std::future<GenerationResult> Client::streamGenerateContentAsync(std::string prompt, StreamCallback callback, std::string_view model_id)
    {
        std::string modelIdStr(model_id);
        return std::async(std::launch::async, [
            this,
            modelIdStr = std::move(modelIdStr),
            callback = std::move(callback),
            prompt = std::move(prompt)]() {
            return streamGenerateContent(prompt, callback, modelIdStr);
        });
    }

    std::future<GenerationResult> Client::streamGenerateContentAsync(std::string prompt, StreamCallback callback, Model model)
    {
        return std::async(std::launch::async, [
            this,
            model,
            callback = std::move(callback),
            prompt = std::move(prompt)]() {
            return streamGenerateContent(prompt, callback, model);
        });
    }

    std::future<Result<File>> Client::uploadFileAsync(std::string path, std::string displayName)
    {
        return std::async(std::launch::async, [this, path = std::move(path), displayName = std::move(displayName)]() {
            return uploadFile(path, displayName);
        });
    }

    std::future<Result<File>> Client::getFileAsync(std::string name)
    {
        return std::async(std::launch::async, [this, name = std::move(name)]() {
            return getFile(name);
        });
    }

    std::future<Result<bool>> Client::deleteFileAsync(std::string name)
    {
        return std::async(std::launch::async, [this, name = std::move(name)]() {
            return deleteFile(name);
        });
    }

    std::future<Result<ListFilesResponse>> Client::listFilesAsync(int pageSize, std::string pageToken)
    {
        return std::async(std::launch::async, [this, pageSize, pageToken = std::move(pageToken)]() {
            return listFiles(pageSize, pageToken);
        });
    }

    std::future<Result<ModelInfo>> Client::getModelInfoAsync(Model model)
    {
        return std::async(std::launch::async, [this, model]() {
            return getModelInfo(model);
        });
    }

    std::future<Result<ModelInfo>> Client::getModelInfoAsync(std::string_view model_id)
    {
        std::string modelIdStr(model_id);
        return std::async(std::launch::async, [this, modelIdStr = std::move(modelIdStr)]() {
            return getModelInfo(modelIdStr);
        });
    }

    std::future<Result<std::vector<ModelInfo>>> Client::listModelsAsync()
    {
        return std::async(std::launch::async, [this]() {
            return listModels();
        });
    }

    std::future<Support::ApiValidationResult> Client::verifyApiKeyAsync()
    {
        return std::async(std::launch::async, [this]() {
            return verifyApiKey();
        });
    }

    std::future<Result<EmbedContentResponse>> Client::embedContentAsync(std::string_view model, std::string text, EmbedRequestBody config)
    {
        std::string modelIdStr(model);
        return std::async(std::launch::async, [this, modelIdStr = std::move(modelIdStr), text = std::move(text), config = std::move(config)]() {
            return embedContent(modelIdStr, text, config);
        });
    }

    std::future<Result<EmbedContentResponse>> Client::embedContentAsync(Model model, std::string text, EmbedRequestBody config)
    {
        return std::async(std::launch::async, [this, model, text = std::move(text), config = std::move(config)]() {
            return embedContent(model, text, config);
        });
    }

    std::future<Result<BatchEmbedContentsResponse>> Client::batchEmbedContentsAsync(std::string_view model, std::vector<std::string> texts, EmbedRequestBody config)
    {
        std::string modelIdStr(model);
        return std::async(std::launch::async, [this, modelIdStr = std::move(modelIdStr), texts = std::move(texts), config = std::move(config)]() {
            return batchEmbedContents(modelIdStr, texts, config);
        });
    }

    std::future<Result<BatchEmbedContentsResponse>> Client::batchEmbedContentsAsync(Model model, std::vector<std::string> texts, EmbedRequestBody config)
    {
        return std::async(std::launch::async, [this, model, texts = std::move(texts), config = std::move(config)]() {
            return batchEmbedContents(model, texts, config);
        });
    }

    std::future<Result<CountTokensResponseBody>> Client::countTokensAsync(std::string_view model, std::vector<Content> contents, std::string systemInstruction, std::vector<Tool> tools)
    {
        std::string modelIdStr(model);
        return std::async(std::launch::async, [this, modelIdStr = std::move(modelIdStr), contents = std::move(contents), systemInstruction = std::move(systemInstruction), tools = std::move(tools)]() {
            return countTokens(modelIdStr, contents, systemInstruction, tools);
        });
    }

    std::future<Result<CountTokensResponseBody>> Client::countTokensAsync(Model model, std::vector<Content> contents, std::string systemInstruction, std::vector<Tool> tools)
    {
        return std::async(std::launch::async, [this, model, contents = std::move(contents), systemInstruction = std::move(systemInstruction), tools = std::move(tools)]() {
            return countTokens(model, contents, systemInstruction, tools);
        });
    }

    std::future<Result<CountTokensResponseBody>> Client::countTokensAsync(std::string_view model, std::string text)
    {
        std::string modelIdStr(model);
        return std::async(std::launch::async, [this, modelIdStr = std::move(modelIdStr), text = std::move(text)]() {
            return countTokens(modelIdStr, text);
        });
    }

    std::future<Result<CountTokensResponseBody>> Client::countTokensAsync(Model model, std::string text)
    {
        return std::async(std::launch::async, [this, model, text = std::move(text)]() {
            return countTokens(model, text);
        });
    }

    std::future<Result<CachedContent>> Client::createCachedContentAsync(const CachedContent& contentConfig)
    {
        return std::async(std::launch::async, [this, contentConfig]() { return createCachedContent(contentConfig); });
    }
    std::future<Result<CachedContent>> Client::getCachedContentAsync(const std::string& name)
    {
        return std::async(std::launch::async, [this, name]() { return getCachedContent(name); });
    }
    std::future<Result<ListCachedContentsResponse>> Client::listCachedContentsAsync(int pageSize, std::string pageToken)
    {
        return std::async(std::launch::async, [this, pageSize, pageToken]() { return listCachedContents(pageSize, pageToken); });
    }
    std::future<Result<bool>> Client::deleteCachedContentAsync(const std::string& name)
    {
        return std::async(std::launch::async, [this, name]() { return deleteCachedContent(name); });
    }

    void Client::setRetryConfig(const Support::RetryConfig& config)
    {
        retryConfig_ = config;
    }

    const Support::RetryConfig& Client::getRetryConfig() const
    {
        return retryConfig_;
    }

    GenerationResult Client::submitRequest(const Url& url, const nlohmann::json& payload)
    {
        int attempt = 0;

        while (true)
        {
            cpr::Response r = cpr::Post(
                cpr::Url{url},
                cpr::Header{
                    {"Content-Type", "application/json"},
                    {"x-goog-api-key", api_key_}
                },
                cpr::Body{payload.dump()}
            );

            if (HttpStatusHelper::isSuccess(r.status_code))
            {
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
                                return GenerationResult::Failure("Prompt blocked by Safety Filter", frenum::value(HttpMappedStatusCode::OK), FinishReason::PROMPT_BLOCKED);
                            }
                        }
                    }
                    
                    if (!json_response.contains("candidates") || json_response["candidates"].empty())
                    {
                        return GenerationResult::Failure("No candidates returned", frenum::value(HttpMappedStatusCode::OK));
                    }
            
                    auto candidate = json_response["candidates"][0];
        
                    FinishReason reason = FinishReason::FINISH_REASON_UNSPECIFIED;
                    if (candidate.contains("finishReason"))
                    {
                        reason = frenum::cast<FinishReason>(candidate["finishReason"].get<std::string>()).value_or(FinishReason::FINISH_REASON_UNSPECIFIED);
                    }
        
                    if (reason == FinishReason::SAFETY && !candidate.contains("content"))
                    {
                        return GenerationResult::Failure("Response blocked by Safety Filter", frenum::value(HttpMappedStatusCode::OK), FinishReason::SAFETY);
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
                    
                    return GenerationResult::Failure("Candidate has no content", frenum::value(HttpMappedStatusCode::OK), reason);
                }
                catch (const std::exception& e)
                {
                    GEMINI_ERROR("JSON Parse Error: {}", e.what());
                    return GenerationResult::Failure("Parse error: "s + e.what(), r.status_code);
                }
            }
            
            if (HttpStatusHelper::isRetryable(r.status_code) && attempt < retryConfig_.maxRetries)
            {
                int waitMs = calculateWaitTime(retryConfig_, attempt, r);
                
                GEMINI_WARN("API Error [{}]. Retrying in {} ms... (Attempt {}/{})", 
                    r.status_code, waitMs, attempt + 1, retryConfig_.maxRetries);

                std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
                attempt++;
                continue;
            }

            std::string errorMsg = Utils::parseErrorMessage(r.text);
            GEMINI_ERROR("API Error [{}]: {}", r.status_code, errorMsg);
            return GenerationResult::Failure(errorMsg, r.status_code);
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

            auto write_func = [&](std::string data, intptr_t userdata) -> bool
            {
                (void)userdata;

                if (!data.empty())
                    dataReceived = true;
                
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

            if (HttpStatusHelper::isSuccess(r.status_code))
            {
                Content finalContent = Content::Model().text(fullTextAccumulator);
                return GenerationResult::Success(finalContent, r.status_code, inputTokens, outputTokens);
            }

            if (HttpStatusHelper::isRetryable(r.status_code) && attempt < retryConfig_.maxRetries && !dataReceived)
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


    GenerationResult Client::generateFromBuilder(Model model, const std::string& sys_instr,
        const std::string& cachedContent, const std::vector<Part>& parts, const GenerationConfig& config,
        const std::vector<SafetySetting>& safetySettings, const std::vector<Tool>& tools, const std::optional<ToolConfig>& toolConfig)
    {
        Content userContent = Content::User();
        userContent.parts = parts; 
        
        nlohmann::json payload = Internal::PayloadBuilder::build(
            {userContent},
            sys_instr,
            cachedContent,
            config, 
            safetySettings, 
            tools,
            toolConfig
        );

        Url url(ResourceName::Model(std::string(ModelHelper::stringRepresentation(model))), GM_GENERATE_CONTENT);

        return submitRequest(url, payload);
    }
    
    GenerationResult Client::streamFromBuilder(Model model, const std::string& sys_instr, const std::string& cachedContent,
        const std::vector<Part>& parts, const GenerationConfig& config, const std::vector<SafetySetting>& safetySettings,
        const StreamCallback& callback, const std::vector<Tool>& tools, const std::optional<ToolConfig>& toolConfig)
    {
        Content userContent = Content::User();
        userContent.parts = parts;

        nlohmann::json payload = Internal::PayloadBuilder::build(
            {userContent}, 
            sys_instr,
            cachedContent,
            config, 
            safetySettings, 
            tools,
            toolConfig
        );

        Url url(ResourceName::Model(std::string(ModelHelper::stringRepresentation(model))), GM_STREAM_GENERATE_CONTENT);
        url.addQuery("alt", "sse");
        
        return submitStreamRequest(url, payload, callback);
    }

    std::future<GenerationResult> Client::generateFromBuilderAsync(Model model, std::string sys_instr, std::string cachedContent,
        std::vector<Part> parts, GenerationConfig config, std::vector<SafetySetting> safetySettings, std::vector<Tool> tools, std::optional<ToolConfig> toolConfig)
    {
        return std::async(std::launch::async, [
            this,
            model,
            sys_instr = std::move(sys_instr),
            cachedContent = std::move(cachedContent),
            parts = std::move(parts), 
            config = std::move(config), 
            safetySettings = std::move(safetySettings), 
            tools = std::move(tools),
            toolConfig = std::move(toolConfig)]() 
        {
            return generateFromBuilder(model, sys_instr, cachedContent, parts, config, safetySettings, tools, toolConfig);
        });
    }

    std::future<GenerationResult> Client::streamFromBuilderAsync(Model model, std::string sys_instr, std::string cachedContent,
        std::vector<Part> parts, GenerationConfig config, std::vector<SafetySetting> safetySettings, StreamCallback callback,
        std::vector<Tool> tools, std::optional<ToolConfig> toolConfig)
    {
        return std::async(std::launch::async, [
            this,
            model,
            sys_instr = std::move(sys_instr), 
            cachedContent = std::move(cachedContent), 
            parts = std::move(parts), 
            config = std::move(config), 
            safetySettings = std::move(safetySettings),
            callback = std::move(callback),
            tools = std::move(tools),
            toolConfig = std::move(toolConfig)]() 
        {
            return streamFromBuilder(model, sys_instr, cachedContent, parts, config, safetySettings, callback, tools, toolConfig);
        });
    }
}
