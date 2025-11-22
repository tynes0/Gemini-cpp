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

        RequestBuilder request();
        
        ChatSession startChat(Model model = Model::GEMINI_2_5_FLASH);
        
        GenerationResult generateContent(const std::string& prompt, std::string_view model_id);
        GenerationResult generateContent(const std::string& prompt, Model model = Model::GEMINI_2_5_FLASH);

        // (For internal use) We need to make the function that sends the raw request public or friend.
        // For now, let's call it a friend class so ChatSession can access it.
        friend class ChatSession;
        friend class RequestBuilder;
    private:
        // --- ENGINE ---
        // This function handles all the networking work.
        // generateContent -> Calls this.
        // ChatSession -> Calls this.
        GenerationResult submitRequest(const std::string& url, const nlohmann::json& payload);
        
        GenerationResult generateFromBuilder(Model model, const std::string& sys_instr, const std::vector<Part>& parts);
        
        std::string api_key_;
    };

}

#endif // GEMINI_CLIENT_H