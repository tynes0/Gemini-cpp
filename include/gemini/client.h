#pragma once

#ifndef GEMINI_CLIENT_H
#define GEMINI_CLIENT_H

#include <string>
#include <string_view>
#include <future>

#include "model.h"
#include "chat_session.h"
#include "request_builder.h"
#include "response.h"
#include "url.h"
#include "support.h"

#include "types/caching_api_types.h"
#include "types/embeddings_api_types.h"
#include "types/generating_content_api_types.h"
#include "types/files_api_types.h"
#include "types/tokens_api_types.h"

namespace GeminiCPP
{
    class Client
    {
    public:
        explicit Client(std::string api_key);

        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;
        Client(Client&&) = default;
        Client& operator=(Client&&) = default;
        ~Client() = default;

        [[nodiscard]] RequestBuilder request(
        );
        
        [[nodiscard]] ChatSession startChat(
            Model model = Model::GEMINI_2_5_FLASH,
            std::string sessionName = "",
            std::string systemInstruction = ""
        );

        [[nodiscard]] ChatSession startChat(
            std::string_view model,
            std::string sessionName = "",
            std::string systemInstruction = ""
        );
        
        [[nodiscard]] GenerationResult generateContent(
            const std::string& prompt, 
            std::string_view model_id
        );

        [[nodiscard]] GenerationResult generateContent(
            const std::string& prompt, 
            Model model = Model::GEMINI_2_5_FLASH
        );
        
        [[nodiscard]] GenerationResult streamGenerateContent(
            const std::string& prompt, 
            const StreamCallback& callback, 
            std::string_view model_id
        );

        [[nodiscard]] GenerationResult streamGenerateContent(
            const std::string& prompt, 
            const StreamCallback& callback, 
            Model model = Model::GEMINI_2_5_FLASH
        );
        
        // File Upload
        // path: Path on disk (e.g., "C:/video.mp4")
        // displayName: Name to be displayed in the API (Optional)
        [[nodiscard]] Result<File> uploadFile(
            const std::string& path,
            std::string displayName = ""
        );

        // Getting File Information
        // name: ID in "files/..." format
        [[nodiscard]] Result<File> getFile(
            const std::string& name
        );

        [[nodiscard]] Result<bool> deleteFile(
            const std::string& name
        );

        [[nodiscard]] Result<ListFilesResponse> listFiles(
            int pageSize = 10,
            const std::string& pageToken = ""
        );
        
        [[nodiscard]] Result<ModelInfo> getModelInfo(
            Model model
        );
        
        [[nodiscard]] Result<ModelInfo> getModelInfo(
            std::string_view model_id
        );
        
        [[nodiscard]] Result<std::vector<ModelInfo>> listModels(
        );

        [[nodiscard]] Support::ApiValidationResult verifyApiKey(
        );

        [[nodiscard]] Result<EmbedContentResponse> embedContent(
            std::string_view model,
            const std::string& text,
            const EmbedRequestBody& config = {}
        );
        
        [[nodiscard]] Result<EmbedContentResponse> embedContent(
            Model model,
            const std::string& text,
            const EmbedRequestBody& config = {}
        );

        [[nodiscard]] Result<BatchEmbedContentsResponse> batchEmbedContents(
            std::string_view model,
            const std::vector<std::string>& texts,
            const EmbedRequestBody& config = {}
        );
        
        [[nodiscard]] Result<BatchEmbedContentsResponse> batchEmbedContents(
            Model model,
            const std::vector<std::string>& texts,
            const EmbedRequestBody& config = {}
        );

        [[nodiscard]] Result<CountTokensResponseBody> countTokens(
            std::string_view model, 
            const std::vector<Content>& contents,
            const std::string& systemInstruction = "",
            const std::vector<Tool>& tools = {}
        );
        
        [[nodiscard]] Result<CountTokensResponseBody> countTokens(
            Model model, 
            const std::vector<Content>& contents,
            const std::string& systemInstruction = "",
            const std::vector<Tool>& tools = {}
        );

        [[nodiscard]] Result<CountTokensResponseBody> countTokens(
            std::string_view model,
            const std::string& text
        );
        
        [[nodiscard]] Result<CountTokensResponseBody> countTokens(
            Model model,
            const std::string& text
        );

        [[nodiscard]] Result<CachedContent> createCachedContent(
            const CachedContent& contentConfig
        );
        
        [[nodiscard]] Result<CachedContent> getCachedContent(
            const std::string& name
        );

        [[nodiscard]] Result<ListCachedContentsResponse> listCachedContents(
            int pageSize = 10, 
            const std::string& pageToken = ""
        );

        [[nodiscard]] Result<bool> deleteCachedContent(
            const std::string& name
        );

        [[nodiscard]] Result<CachedContent> updateCachedContent(
            const std::string& name,
            const std::string& ttl
        );

        // --- ASYNC METHODS (NON-BLOCKING) ---
        [[nodiscard]] std::future<GenerationResult> generateContentAsync(
            std::string prompt,
            std::string_view model_id
        );

        [[nodiscard]] std::future<GenerationResult> generateContentAsync(
            std::string prompt,
            Model model = Model::GEMINI_2_5_FLASH
        );
        
        [[nodiscard]] std::future<GenerationResult> streamGenerateContentAsync(
            std::string prompt,
            StreamCallback callback,
            std::string_view model_id
        );

        [[nodiscard]] std::future<GenerationResult> streamGenerateContentAsync(
            std::string prompt,
            StreamCallback callback,
            Model model = Model::GEMINI_2_5_FLASH
        );

        // --- FILES API ASYNC ---
        [[nodiscard]] std::future<Result<File>> uploadFileAsync(
            std::string path,
            std::string displayName = ""
        );

        [[nodiscard]] std::future<Result<File>> getFileAsync(
            std::string name
        );

        [[nodiscard]] std::future<Result<bool>> deleteFileAsync(
            std::string name
        );

        [[nodiscard]] std::future<Result<ListFilesResponse>> listFilesAsync(
            int pageSize = 10,
            std::string pageToken = ""
        );
        
        // --- MODEL INFO ASYNC ---
        [[nodiscard]] std::future<Result<ModelInfo>> getModelInfoAsync(
            Model model
        );
        
        [[nodiscard]] std::future<Result<ModelInfo>> getModelInfoAsync(
            std::string_view model_id
        );
        
        [[nodiscard]] std::future<Result<std::vector<ModelInfo>>> listModelsAsync(
        );

        [[nodiscard]] std::future<Support::ApiValidationResult> verifyApiKeyAsync(
        );

        // --- EMBEDDINGS & TOKENS ASYNC ---
        [[nodiscard]] std::future<Result<EmbedContentResponse>> embedContentAsync(
            std::string_view model,
            std::string text,
            EmbedRequestBody config = {}
        );
        
        [[nodiscard]] std::future<Result<EmbedContentResponse>> embedContentAsync(
            Model model,
            std::string text,
            EmbedRequestBody config = {}
        );

        [[nodiscard]] std::future<Result<BatchEmbedContentsResponse>> batchEmbedContentsAsync(
            std::string_view model,
            std::vector<std::string> texts,
            EmbedRequestBody config = {}
        );
        
        [[nodiscard]] std::future<Result<BatchEmbedContentsResponse>> batchEmbedContentsAsync(
            Model model,
            std::vector<std::string> texts,
            EmbedRequestBody config = {}
        );

        [[nodiscard]] std::future<Result<CountTokensResponseBody>> countTokensAsync(
            std::string_view model, 
            std::vector<Content> contents,
            std::string systemInstruction = "",
            std::vector<Tool> tools = {}
        );
        
        [[nodiscard]] std::future<Result<CountTokensResponseBody>> countTokensAsync(
            Model model, 
            std::vector<Content> contents,
            std::string systemInstruction = "",
            std::vector<Tool> tools = {}
        );

        [[nodiscard]] std::future<Result<CountTokensResponseBody>> countTokensAsync(
            std::string_view model,
            std::string text
        );
        
        [[nodiscard]] std::future<Result<CountTokensResponseBody>> countTokensAsync(
            Model model,
            std::string text
        );

        [[nodiscard]] std::future<Result<CachedContent>> createCachedContentAsync(
            const CachedContent& contentConfig
        );
        
        [[nodiscard]] std::future<Result<CachedContent>> getCachedContentAsync(
            const std::string& name
        );
        
        [[nodiscard]] std::future<Result<ListCachedContentsResponse>> listCachedContentsAsync(
            int pageSize = 10,
            std::string pageToken = ""
        );
        
        [[nodiscard]] std::future<Result<bool>> deleteCachedContentAsync(
            const std::string& name
        );

        void setRetryConfig(
            const Support::RetryConfig& config
        );
        
        [[nodiscard]] const Support::RetryConfig& getRetryConfig(
        ) const;
        
        friend class ChatSession;
        friend class RequestBuilder;
    private:
        // --- ENGINE ---
        // This functions handles all the networking work.
        
        [[nodiscard]] GenerationResult submitRequest(
            const Url&  url,
            const nlohmann::json& payload
        );

        [[nodiscard]] GenerationResult submitStreamRequest(
            const Url& url,
            const nlohmann::json& payload,
            const StreamCallback& callback
        );
        
        [[nodiscard]] GenerationResult generateFromBuilder(
            Model model,
            const std::string& sys_instr,
            const std::string& cachedContent,
            const std::vector<Part>& parts,
            const GenerationConfig& config = {},
            const std::vector<SafetySetting>& safetySettings = {},
            const std::vector<Tool>& tools = {},
            const std::optional<ToolConfig>& toolConfig = std::nullopt
        );
        
        [[nodiscard]] GenerationResult streamFromBuilder(
            Model model,
            const std::string& sys_instr,
            const std::string& cachedContent,
            const std::vector<Part>& parts,
            const GenerationConfig& config,
            const std::vector<SafetySetting>& safetySettings,
            const StreamCallback& callback,
            const std::vector<Tool>& tools = {},
            const std::optional<ToolConfig>& toolConfig = std::nullopt
        );

        [[nodiscard]] std::future<GenerationResult> generateFromBuilderAsync(
            Model model,
            std::string sys_instr,
            std::string cachedContent,
            std::vector<Part> parts,
            GenerationConfig config,
            std::vector<SafetySetting> safetySettings,
            std::vector<Tool> tools,
            std::optional<ToolConfig> toolConfig = std::nullopt
        );

        [[nodiscard]] std::future<GenerationResult> streamFromBuilderAsync(
            Model model,
            std::string sys_instr,
            std::string cachedContent,
            std::vector<Part> parts,
            GenerationConfig config,
            std::vector<SafetySetting> safetySettings,
            StreamCallback callback,
            std::vector<Tool> tools,
            std::optional<ToolConfig> toolConfig = std::nullopt
        );
        
        std::string api_key_;
        Support::RetryConfig retryConfig_;
    };

}

#endif // GEMINI_CLIENT_H