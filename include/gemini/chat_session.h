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

    /**
     * @brief Manages a multi-turn conversation with the Gemini model.
     * * This class maintains the history of the conversation, handles function calling loops
     * (auto-reply), and manages session-specific configurations like safety settings and tools.
     * It is thread-safe for modifying settings and sending messages.
     */
    class ChatSession 
    {
    public:
        /**
         * @brief Constructs a new ChatSession.
         * @param client Pointer to the Client instance (must remain valid).
         * @param model The model enum to use.
         * @param sessionName Optional display name for the session.
         * @param sessionId Optional ID (if empty, a UUID is generated).
         */
        ChatSession(Client* client, Model model, std::string sessionName = "", std::string sessionId = "");
        
        /**
         * @brief Constructs a new ChatSession using a string for the model name.
         * @param client Pointer to the Client instance.
         * @param model The string representation of the model (e.g., "gemini-1.5-pro").
         * @param sessionName Optional display name.
         * @param sessionId Optional ID.
         */
        ChatSession(Client* client, std::string_view model, std::string sessionName = "", std::string sessionId = "");
        
        ChatSession(const ChatSession& other);
        ChatSession& operator=(const ChatSession& other);

        ~ChatSession() = default;
        
        /**
         * @brief Sends a structured content message to the chat.
         * @details Updates history and handles potential function call loops if auto-reply is enabled.
         * @param content The content to send.
         * @return GenerationResult containing the model's response.
         */
        [[nodiscard]] GenerationResult send(const Content& content);

        /**
         * @brief Sends a simple text message to the chat.
         * @param text The text to send.
         * @return GenerationResult containing the model's response.
         */
        [[nodiscard]] GenerationResult send(const std::string& text);

        /**
         * @brief Streams the response for a structured content message.
         * @param content The content to send.
         * @param callback The function to call for each received chunk.
         * @return GenerationResult containing the aggregated response.
         */
        [[nodiscard]] GenerationResult stream(const Content& content, const StreamCallback& callback);

        /**
         * @brief Streams the response for a text message.
         * @param text The text to send.
         * @param callback The function to call for each received chunk.
         * @return GenerationResult containing the aggregated response.
         */
        [[nodiscard]] GenerationResult stream(const std::string& text, const StreamCallback& callback);

        // Async versions
        [[nodiscard]] std::future<GenerationResult> sendAsync(const Content& content);
        [[nodiscard]] std::future<GenerationResult> sendAsync(const std::string& text);
        [[nodiscard]] std::future<GenerationResult> streamAsync(const Content& content, const StreamCallback& callback);
        [[nodiscard]] std::future<GenerationResult> streamAsync(const std::string& text, const StreamCallback& callback);

        /**
         * @brief Gets the unique session ID.
         */
        [[nodiscard]] std::string getId() const;
        
        /**
         * @brief Updates the model used for this session.
         */
        void setModel(Model model);
        void setModel(std::string_view model);
        [[nodiscard]] std::string getModel() const;
        
        /**
         * @brief Sets the system instruction for the session.
         * @note System instructions are usually set at the beginning of a session.
         */
        void setSystemInstruction(std::string systemInstruction);
        [[nodiscard]] std::string getSystemInstruction() const;
        
        void setSessionName(std::string sessionName);
        [[nodiscard]] std::string getSessionName() const;

        /**
         * @brief Sets a cached content resource name to be used as context.
         * @param cacheName resource name (e.g., "cachedContents/xxx").
         */
        void setCachedContent(std::string cacheName);
        [[nodiscard]] std::string getCachedContent() const;

        /**
         * @brief Overrides the generation configuration for this session.
         */
        void setConfig(const GenerationConfig& config);
        [[nodiscard]] GenerationConfig& config();
        
        // Tool Management
        void addTool(const Tool& tool);
        void setTools(const std::vector<Tool>& tools);
        void clearTools();

        // Safety Settings
        void setSafetySettings(const std::vector<SafetySetting>& settings);
        void addSafetySetting(HarmCategory category, HarmBlockThreshold threshold);
        void clearSafetySettings();

        /**
         * @brief Registers a client-side function that the model can call.
         * @tparam Func The type of the function/lambda.
         * @param name The name of the function (as exposed to the model).
         * @param func The function implementation.
         * @param doc A description of what the function does.
         * @param argNames The names of the function arguments.
         */
        template <typename Func>
        void registerFunction(const std::string& name, Func&& func, const std::string& doc, const std::vector<std::string>& argNames)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            functionRegistry_.registerFunction(name, std::forward<Func>(func), doc, argNames);
        }

        /**
         * @brief Enables or disables automatic execution of function calls.
         * @param enabled If true, the client executes tool calls and sends results back automatically.
         */
        void setAutoReply(bool enabled);
        
        /**
         * @brief Sets the maximum number of consecutive function call turns allowed.
         * @param maxTurns Maximum turns (default 10) to prevent infinite loops.
         */
        void setMaxFunctionCallTurns(int maxTurns);
        [[nodiscard]] int getMaxFunctionCallTurns() const;
        
        /**
         * @brief Clears the conversation history. System instructions are preserved.
         */
        void clearHistory();
        
        /**
         * @brief Retrieves the current conversation history.
         */
        [[nodiscard]] std::vector<Content> history() const;
        
        // Serialization
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