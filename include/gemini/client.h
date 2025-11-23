#pragma once

#ifndef GEMINI_CLIENT_H
#define GEMINI_CLIENT_H

#include <string>
#include <string_view>
#include "model.h"
#include "chat_session.h"
#include "request_builder.h"
#include "response.h"

namespace GeminiCPP
{
    class Client
    {
    public:
        explicit Client(std::string api_key);

        [[nodiscard]] RequestBuilder request();
        
        [[nodiscard]] ChatSession startChat(Model model = Model::GEMINI_2_5_FLASH, std::string systemInstruction = "");
        
        [[nodiscard]] GenerationResult generateContent(const std::string& prompt, std::string_view model_id);
        [[nodiscard]] GenerationResult generateContent(const std::string& prompt, Model model = Model::GEMINI_2_5_FLASH);
        
        [[nodiscard]] GenerationResult streamGenerateContent(const std::string& prompt, const StreamCallback& callback, std::string_view model_id);
        [[nodiscard]] GenerationResult streamGenerateContent(const std::string& prompt, const StreamCallback& callback, Model model = Model::GEMINI_2_5_FLASH);

        [[nodiscard]] std::optional<ModelInfo> getModelInfo(Model model);
        [[nodiscard]] std::optional<ModelInfo> getModelInfo(std::string_view model_id);

        // (For internal use) We need to make the function that sends the raw request public or friend.
        // For now, let's call it a friend class so ChatSession can access it.
        friend class ChatSession;
        friend class RequestBuilder;
    private:
        // --- ENGINE ---
        // This function handles all the networking work.
        // generateContent -> Calls this.
        // ChatSession -> Calls this.
        [[nodiscard]] GenerationResult submitRequest(
            const std::string& url,
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
            const std::string& url,
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