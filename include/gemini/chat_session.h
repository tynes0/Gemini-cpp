#pragma once

#ifndef GEMINI_CHAT_SESSION_H
#define GEMINI_CHAT_SESSION_H

#include <future>
#include <vector>
#include <string>
#include <mutex>

#include "model.h"
#include "response.h"
#include "function_registry.h"
#include "support.h"

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

        ~ChatSession() = default;
        
        [[nodiscard]] GenerationResult send(const Content& content);
        [[nodiscard]] GenerationResult send(const std::string& text);

        [[nodiscard]] GenerationResult stream(const Content& content, const StreamCallback& callback);
        [[nodiscard]] GenerationResult stream(const std::string& text, const StreamCallback& callback);

        [[nodiscard]] std::future<GenerationResult> sendAsync(const Content& content);
        [[nodiscard]] std::future<GenerationResult> sendAsync(const std::string& text);

        [[nodiscard]] std::future<GenerationResult> streamAsync(const Content& content, const StreamCallback& callback);
        [[nodiscard]] std::future<GenerationResult> streamAsync(const std::string& text, const StreamCallback& callback);

        [[nodiscard]] std::string getId() const;
        
        void setModel(Model model);
        void setModel(std::string_view model);
        [[nodiscard]] std::string getModel() const;
        
        void setSystemInstruction(std::string systemInstruction);
        [[nodiscard]] std::string getSystemInstruction() const;
        
        void setSessionName(std::string sessionName);
        [[nodiscard]] std::string getSessionName() const;

        void setCachedContent(std::string cacheName);
        [[nodiscard]] std::string getCachedContent() const;

        void setConfig(const GenerationConfig& config);
        [[nodiscard]] GenerationConfig& config();
        
        void addTool(const Tool& tool);
        void setTools(const std::vector<Tool>& tools);
        void clearTools();

        void setSafetySettings(const std::vector<SafetySetting>& settings);
        void addSafetySetting(HarmCategory category, HarmBlockThreshold threshold);
        void clearSafetySettings();

        template <typename Func>
        void registerFunction(const std::string& name, Func&& func, const std::string& doc, const std::vector<std::string>& argNames)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            functionRegistry_.registerFunction(name, std::forward<Func>(func), doc, argNames);
        }

        void setAutoReply(bool enabled);
        
        void setMaxFunctionCallTurns(int maxTurns);
        [[nodiscard]] int getMaxFunctionCallTurns() const;
        
        void clearHistory();
        
        [[nodiscard]] std::vector<Content> history() const;
        
        [[nodiscard]] nlohmann::json toJson() const;
        [[nodiscard]] static ChatSession fromJson(Client* client, const nlohmann::json& j);
    private:
        [[nodiscard]] GenerationResult sendInternal();
        [[nodiscard]] GenerationResult streamInternal(const StreamCallback& callback);
        [[nodiscard]] std::vector<Tool> getCombinedTools() const;

        Client* client_;
        std::string model_;
        std::string sessionId_;
        std::string sessionName_;
        std::string systemInstruction_;
        std::string cachedContent_;
        std::vector<Content> history_;
        std::vector<Tool> tools_;
        FunctionRegistry functionRegistry_;
        bool autoReply_ = true;
        int maxFunctionCallTurns_ = 10;

        GenerationConfig config_;
        std::vector<SafetySetting> safetySettings_;

        mutable std::mutex mutex_;
    };
}
#endif