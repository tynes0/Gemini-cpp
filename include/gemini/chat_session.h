#pragma once

#ifndef GEMINI_CHAT_SESSION_H
#define GEMINI_CHAT_SESSION_H

#include <vector>
#include <string>
#include <mutex>
#include "types.h"
#include "model.h"
#include "response.h"

namespace GeminiCPP
{
    class Client; 

    class ChatSession 
    {
    public:
        ChatSession(Client* client, Model model, std::string systemInstruction = "", std::string sessionId = "");
        ChatSession(const ChatSession& other);
        
        [[nodiscard]] GenerationResult send(const Content& content);
        [[nodiscard]] GenerationResult send(const std::string& text);

        [[nodiscard]] GenerationResult stream(const Content& content, const StreamCallback& callback);
        [[nodiscard]] GenerationResult stream(const std::string& text, const StreamCallback& callback);

        void addTool(const Tool& tool);
        void setTools(const std::vector<Tool>& tools);
        
        void changeModel(Model model);
        void changeSystemInstruction(std::string systemInstruction);
        void clearHistory();
        
        [[nodiscard]] const std::vector<Content>& history() const;

        [[nodiscard]] std::string getId() const;
        
        [[nodiscard]] nlohmann::json toJson() const;
        [[nodiscard]] static ChatSession fromJson(Client* client, const nlohmann::json& j);
    private:
        [[nodiscard]] GenerationResult sendInternal();
        [[nodiscard]] GenerationResult streamInternal(const StreamCallback& callback);

        Client* client_;
        Model model_;
        std::string sessionId_;
        std::string systemInstruction_;
        std::vector<Content> history_;
        std::vector<Tool> tools_;

        mutable std::mutex mutex_;
    };
}
#endif