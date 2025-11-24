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
        ChatSession(Client* client, Model model, std::string sessionName = "", std::string systemInstruction = "", std::string sessionId = "");
        ChatSession(Client* client, std::string_view model, std::string sessionName = "", std::string systemInstruction = "", std::string sessionId = "");
        
        ChatSession(const ChatSession& other);
        ChatSession& operator=(const ChatSession& other);
        
        [[nodiscard]] GenerationResult send(const Content& content);
        [[nodiscard]] GenerationResult send(const std::string& text);

        [[nodiscard]] GenerationResult stream(const Content& content, const StreamCallback& callback);
        [[nodiscard]] GenerationResult stream(const std::string& text, const StreamCallback& callback);

        void setModel(Model model);
        void setModel(std::string_view model);
        void setSystemInstruction(std::string systemInstruction);
        void setSessionName(std::string sessionName);

        void setConfig(const GenerationConfig& config);
        GenerationConfig& config();
        
        void addTool(const Tool& tool);
        void setTools(const std::vector<Tool>& tools);
        void clearTools();

        void setSafetySettings(const std::vector<SafetySetting>& settings);
        void addSafetySetting(HarmCategory category, HarmBlockThreshold threshold);
        void clearSafetySettings();
        
        void clearHistory();
        
        [[nodiscard]] std::vector<Content> history() const;

        [[nodiscard]] std::string getId() const;
        [[nodiscard]] std::string getName() const;
        
        [[nodiscard]] nlohmann::json toJson() const;
        [[nodiscard]] static ChatSession fromJson(Client* client, const nlohmann::json& j);
    private:
        [[nodiscard]] GenerationResult sendInternal();
        [[nodiscard]] GenerationResult streamInternal(const StreamCallback& callback);

        Client* client_;
        std::string model_;
        std::string sessionId_;
        std::string sessionName_;
        std::string systemInstruction_;
        std::vector<Content> history_;
        std::vector<Tool> tools_;

        GenerationConfig config_;
        std::vector<SafetySetting> safetySettings_;

        mutable std::mutex mutex_;
    };
}
#endif