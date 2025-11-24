#pragma once

#ifndef GEMINI_CLIENT_H
#define GEMINI_CLIENT_H

#include <string>
#include <string_view>
#include "model.h"
#include "chat_session.h"
#include "request_builder.h"
#include "response.h"
#include "url.h"

namespace GeminiCPP
{
    class Client
    {
    public:
        explicit Client(std::string api_key);

        [[nodiscard]] RequestBuilder request();
        
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
            std::string_view model_id,
            const GenerationConfig& config = {}, 
            const std::vector<SafetySetting>& safetySettings = {}
        );

        [[nodiscard]] GenerationResult generateContent(
            const std::string& prompt, 
            Model model = Model::GEMINI_2_5_FLASH,
            const GenerationConfig& config = {}, 
            const std::vector<SafetySetting>& safetySettings = {}
        );
        
        [[nodiscard]] GenerationResult streamGenerateContent(
            const std::string& prompt, 
            const StreamCallback& callback, 
            std::string_view model_id,
            const GenerationConfig& config = {}, 
            const std::vector<SafetySetting>& safetySettings = {}
        );

        [[nodiscard]] GenerationResult streamGenerateContent(
            const std::string& prompt, 
            const StreamCallback& callback, 
            Model model = Model::GEMINI_2_5_FLASH,
            const GenerationConfig& config = {}, 
            const std::vector<SafetySetting>& safetySettings = {}
        );
        [[nodiscard]] Result<ModelInfo> getModelInfo(
            Model model
        );
        
        [[nodiscard]] Result<ModelInfo> getModelInfo(
            std::string_view model_id
        );
        
        [[nodiscard]] Result<std::vector<ModelInfo>> listModels();

        [[nodiscard]] ApiValidationResult verifyApiKey();

        [[nodiscard]] Result<EmbedContentResponse> embedContent(
            std::string_view model,
            const std::string& text,
            const EmbedConfig& config = {}
        );
        
        [[nodiscard]] Result<EmbedContentResponse> embedContent(
            Model model,
            const std::string& text,
            const EmbedConfig& config = {}
        );

        [[nodiscard]] Result<BatchEmbedContentsResponse> batchEmbedContents(
            std::string_view model,
            const std::vector<std::string>& texts,
            const EmbedConfig& config = {}
        );
        
        [[nodiscard]] Result<BatchEmbedContentsResponse> batchEmbedContents(
            Model model,
            const std::vector<std::string>& texts,
            const EmbedConfig& config = {}
        );

        [[nodiscard]] Result<TokenCountResponse> countTokens(
            std::string_view model, 
            const std::vector<Content>& contents,
            const std::string& systemInstruction = "",
            const std::vector<Tool>& tools = {}
        );
        
        [[nodiscard]] Result<TokenCountResponse> countTokens(
            Model model, 
            const std::vector<Content>& contents,
            const std::string& systemInstruction = "",
            const std::vector<Tool>& tools = {}
        );

        [[nodiscard]] Result<TokenCountResponse> countTokens(
            std::string_view model,
            const std::string& text
        );
        
        [[nodiscard]] Result<TokenCountResponse> countTokens(
            Model model,
            const std::string& text
        );
        
        // (For internal use) We need to make the function that sends the raw request public or friend.
        // For now, let's call it a friend class so ChatSession can access it.
        friend class ChatSession;
        friend class RequestBuilder;
    private:
        // --- ENGINE ---
        // This functions handles all the networking work.
        
        [[nodiscard]] GenerationResult submitRequest(
            const Url&  url,
            const nlohmann::json& payload
            );
        
        [[nodiscard]] GenerationResult generateFromBuilder(
            Model model,
            const std::string& sys_instr,
            const std::vector<Part>& parts,
            const GenerationConfig& config = {},
            const std::vector<SafetySetting>& safetySettings = {},
            const std::vector<Tool>& tools = {}
            );

        [[nodiscard]] GenerationResult submitStreamRequest(
            const Url& url,
            const nlohmann::json& payload,
            const StreamCallback& callback
            );
        
        [[nodiscard]] GenerationResult streamFromBuilder(
            Model model,
            const std::string& sys_instr,
            const std::vector<Part>& parts,
            const GenerationConfig& config,
            const std::vector<SafetySetting>& safetySettings,
            const StreamCallback& callback,
            const std::vector<Tool>& tools = {}
            );
        
        std::string api_key_;
    };

}

#endif // GEMINI_CLIENT_H