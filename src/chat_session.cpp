#include "gemini/chat_session.h"
#include "gemini/client.h"
#include <nlohmann/json.hpp>

#include "gemini/uuid.h"
#include "gemini/logger.h"
#include "internal/payload_builder.h"

namespace GeminiCPP
{
    ChatSession::ChatSession(Client* client, Model model, std::string sessionName, std::string systemInstruction, std::string sessionId)
        : client_(client), model_(modelStringRepresentation(model)), sessionName_(std::move(sessionName)), systemInstruction_(std::move(systemInstruction))
    {
        sessionId_ = sessionId.empty() ? Uuid::generate() : std::move(sessionId);
    }

    ChatSession::ChatSession(Client* client, std::string_view model, std::string sessionName, std::string systemInstruction, std::string sessionId)
        : client_(client), model_(model), sessionId_(std::move(sessionId)), sessionName_(std::move(sessionName)), systemInstruction_(std::move(systemInstruction))
    {
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
        {
            std::lock_guard<std::mutex> lock(mutex_);
            history_.push_back(content);
        }

        return sendInternal();
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

    void ChatSession::setModel(Model model)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        model_ = modelStringRepresentation(model);
    }

    void ChatSession::setModel(std::string_view model)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        model_ = model;
    }

    void ChatSession::setSystemInstruction(std::string systemInstruction)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        systemInstruction_ = std::move(systemInstruction);
    }

    void ChatSession::setSessionName(std::string sessionName)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        sessionName_ = std::move(sessionName);
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
        safetySettings_.emplace_back(category, threshold);
    }

    void ChatSession::clearSafetySettings()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        safetySettings_.clear();
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

    std::string ChatSession::getId() const
    {
        return sessionId_;
    }

    std::string ChatSession::getName() const
    {
        return sessionName_;
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
            j.value("systemInstruction", ""),
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
        {
            GEMINI_ERROR("Error: Client is null");
            return GenerationResult::Failure("Client is null");
        }

        nlohmann::json payload;
        Url url;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            
            payload = Internal::PayloadBuilder::build(
                history_, 
                systemInstruction_, 
                config_,
                safetySettings_,
                tools_
            );

            url = Url(ResourceName::Model(model_), ":generateContent");
        }

        auto result = client_->submitRequest(url, payload);

        if (result.success)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            history_.push_back(result.content);
        }

        return result;
    }

    GenerationResult ChatSession::streamInternal(const StreamCallback& callback)
    {
        if (!client_)
        {
            GEMINI_ERROR("Error: Client is null");
            return GenerationResult::Failure("Client is null");
        }
        nlohmann::json payload;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            payload = Internal::PayloadBuilder::build(
                history_, 
                systemInstruction_, 
                config_, 
                safetySettings_, 
                tools_
            );
        }
        
        Url url(ResourceName::Model(model_), ":streamGenerateContent");
        url.addQuery("alt", "sse");

        GenerationResult result = client_->submitStreamRequest(url, payload, callback);

        if (result.success)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            history_.push_back(result.content);
        }

        return result;
    }
}
