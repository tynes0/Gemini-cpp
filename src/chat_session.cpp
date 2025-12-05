#include "gemini/chat_session.h"
#include "gemini/client.h"
#include <nlohmann/json.hpp>

#include "gemini/uuid.h"
#include "gemini/logger.h"
#include "gemini/storage.h"

namespace GeminiCPP
{
    ChatSession::ChatSession(Client* client, std::string_view model, std::string sessionName, std::string sessionId)
        : client_(client), model_(model), sessionName_(std::move(sessionName))
    {
        sessionId_ = sessionId.empty() ? Uuid::generate() : std::move(sessionId);
    }

    ChatSession::ChatSession(const ChatSession& other)
        : client_(other.client_), model_(other.model_), sessionId_(other.sessionId_),
        systemInstruction_(other.systemInstruction_), history_(other.history_), tools_(other.tools_)
    {
    }

    ChatSession& ChatSession::operator=(const ChatSession& other)
    {
        if (std::addressof(other) == this)
            return *this;
        client_ = other.client_;
        model_ = other.model_;
        sessionName_ = other.sessionName_;
        systemInstruction_ = other.systemInstruction_;
        history_ = other.history_;
        tools_ = other.tools_;
        return *this;
    }

    GenerationResult ChatSession::send(const Content& content)
    {
        int remainingTurns = 0;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            history_.push_back(content);
            remainingTurns = maxFunctionCallTurns_;
        }
        
        while (remainingTurns--)
        {
            GenerationResult result = sendInternal();

            if (!result.success)
                return result;
            
            bool autoReplyEnabled;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                autoReplyEnabled = autoReply_;
            }
            
            if (!autoReplyEnabled)
                return result;

            // --- FUNCTION CALLING AUTO-REPLY LOGIC ---
            bool hasFunctionCall = false;
            Content functionResponseContent = Content::Function();

            for (const auto& part : result.content.parts)
            {
                if (part.isFunctionCall())
                {
                    hasFunctionCall = true;
                    auto call = part.getFunctionCall();

                    auto jsonResult = functionRegistry_.invoke(call->name, call->args);
                    
                    FunctionResponse response;
                    response.name = call->name;
                    
                    if (jsonResult.has_value())
                    {
                        response.responseContent = jsonResult.value();
                    } else
                    {
                        GEMINI_ERROR("Function invocation failed: {}", call->name);
                        response.responseContent = {
                            {"error", "Function execution failed or not found"}
                        };
                    }
                    functionResponseContent.functionResponse(response);
                }
            }

            if (!hasFunctionCall)
                return result;

            {
                std::lock_guard<std::mutex> lock(mutex_);
                history_.push_back(functionResponseContent);
            }
        }

        return GenerationResult::Failure("Max function call turns exceeded");
    }

    GenerationResult ChatSession::send(const std::string& text)
    {
        return send(Content::User().text(text));
    }

    GenerationResult ChatSession::stream(const Content& content, const StreamCallback& callback)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            history_.push_back(content);
        }
        return streamInternal(callback);
    }

    GenerationResult ChatSession::stream(const std::string& text, const StreamCallback& callback)
    {
        return stream(Content::User().text(text), callback);
    }

    std::future<GenerationResult> ChatSession::sendAsync(const Content& content)
    {
        return std::async(std::launch::async, [this, content]() {
            return this->send(content);
        });
    }

    std::future<GenerationResult> ChatSession::sendAsync(const std::string& text)
    {
        return sendAsync(Content::User().text(text));
    }

    std::future<GenerationResult> ChatSession::streamAsync(const Content& content, const StreamCallback& callback)
    {
        return std::async(std::launch::async, [this, content, callback]() {
            return this->stream(content, callback);
        });
    }

    std::future<GenerationResult> ChatSession::streamAsync(const std::string& text, const StreamCallback& callback)
    {
        return streamAsync(Content::User().text(text), callback);
    }

    std::string ChatSession::getId() const
    {
        return sessionId_;
    }

    void ChatSession::setModel(std::string_view model)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        model_ = model;
    }

    std::string ChatSession::getModel() const
    {
        return model_;
    }

    void ChatSession::setSystemInstruction(std::string systemInstruction)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        systemInstruction_ = std::move(systemInstruction);
    }

    std::string ChatSession::getSystemInstruction() const
    {
        return systemInstruction_;
    }

    void ChatSession::setSessionName(std::string sessionName)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        sessionName_ = std::move(sessionName);
    }

    std::string ChatSession::getSessionName() const
    {
        return sessionName_;
    }
    
    void ChatSession::setCachedContent(std::string cacheName)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        cachedContent_ = std::move(cacheName);
    }

    std::string ChatSession::getCachedContent() const
    {
        return cachedContent_;
    }

    void ChatSession::setConfig(const GenerationConfig& config)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
    }

    GenerationConfig& ChatSession::config()
    {
        // Warning: This method cannot be protected with a mutex because it returns a reference.
        // Once the user receives this reference, they are responsible for thread-safety.
        // However, config settings are usually made outside the chat flow.
        return config_;
    }

    void ChatSession::addTool(const Tool& tool)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tools_.push_back(tool);
    }

    void ChatSession::setTools(const std::vector<Tool>& tools)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tools_ = tools;
    }

    void ChatSession::clearTools()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tools_.clear();
    }

    void ChatSession::setSafetySettings(const std::vector<SafetySetting>& settings)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        safetySettings_ = settings;
    }

    void ChatSession::addSafetySetting(HarmCategory category, HarmBlockThreshold threshold)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        SafetySetting setting;
        setting.category = category;
        setting.threshold = threshold;
        safetySettings_.push_back(setting);
    }

    void ChatSession::clearSafetySettings()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        safetySettings_.clear();
    }

    void ChatSession::setAutoReply(bool enabled)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        autoReply_ = enabled;
    }

    void ChatSession::setMaxFunctionCallTurns(int maxTurns)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        maxFunctionCallTurns_ = maxTurns;
    }

    int ChatSession::getMaxFunctionCallTurns() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return maxFunctionCallTurns_;
    }

    void ChatSession::clearHistory()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        history_.clear();
    }

    std::vector<Content> ChatSession::history() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return history_;
    }
    
    nlohmann::json ChatSession::toJson() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        nlohmann::json j;
        j["id"] = sessionId_;
        j["name"] = sessionName_;
        j["model"] = model_;
        j["systemInstruction"] = systemInstruction_;
        
        nlohmann::json histArr = nlohmann::json::array();
        for(const auto& content : history_) {
            histArr.push_back(content.toJson());
        }
        j["history"] = histArr;

        return j;
    }

    ChatSession ChatSession::fromJson(Client* client, const nlohmann::json& j)
    {
        std::string modelName = j.value("model", "gemini-2.5-flash");

        ChatSession session(
            client,
            modelName,
            j.value("name", ""),
            j.value("id", "")
        );
        
        if(j.contains("history"))
        {
            for(const auto& item : j["history"])
            {
                session.history_.push_back(Content::fromJson(item));
            }
        }
        
        return session;
    }

    GenerationResult ChatSession::sendInternal()
    {
        if (!client_)
            return GenerationResult::Failure("Client is null");

        GenerateContentRequestBody request;
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            
            // Populate Request Body using types
            request.contents = history_;
            
            if (!systemInstruction_.empty())
                request.systemInstruction = Content::User().text(systemInstruction_); // System instruction content
            
            if (!cachedContent_.empty())
                request.cachedContent = ResourceName::CachedContent(cachedContent_);
            
            // Config & Safety
            request.generationConfig = config_;
            if (!safetySettings_.empty())
                request.safetySettings = safetySettings_;
            
            // Tools
            std::vector<Tool> combinedTools = getCombinedTools();
            if (!combinedTools.empty())
                request.tools = combinedTools;
        }

        // Call Client
        GenerationResult result = client_->generateContent(model_, request);

        if (result.success)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            history_.push_back(result.content);
        }

        return result;
    }

    GenerationResult ChatSession::streamInternal(const StreamCallback& callback)
    {
        if (!client_) return GenerationResult::Failure("Client is null");

        StreamGenerateContentRequestBody request;
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            
            request.contents = history_;
            
            if (!systemInstruction_.empty())
                request.systemInstruction = Content::User().text(systemInstruction_);
            
            if (!cachedContent_.empty())
                request.cachedContent = ResourceName::CachedContent(cachedContent_);
            
            request.generationConfig = config_;
            if (!safetySettings_.empty())
                request.safetySettings = safetySettings_;
            
            std::vector<Tool> combinedTools = getCombinedTools();
            if (!combinedTools.empty())
                request.tools = combinedTools;
        }

        GenerationResult result = client_->streamGenerateContent(model_, request, callback);

        if (result.success)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            history_.push_back(result.content);
        }

        return result;
    }

    std::vector<Tool> ChatSession::getCombinedTools() const
    {
        std::vector<Tool> combined = tools_;
        if (!functionRegistry_.empty())
            combined.push_back(functionRegistry_.getTool());
        return combined;
    }
}
