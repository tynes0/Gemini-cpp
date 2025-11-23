#include "gemini/chat_session.h"
#include "gemini/client.h"
#include <nlohmann/json.hpp>

#include "gemini/uuid.h"
#include "internal/payload_builder.h"

namespace GeminiCPP
{
    ChatSession::ChatSession(Client* client, Model model, std::string systemInstruction, std::string sessionId)
        : client_(client), model_(model), systemInstruction_(std::move(systemInstruction))
    {
        sessionId_ = sessionId.empty() ? Uuid::generate() : std::move(sessionId);
    }

    ChatSession::ChatSession(const ChatSession& other)
        : client_(other.client_), model_(other.model_), sessionId_(other.sessionId_),
        systemInstruction_(other.systemInstruction_), history_(other.history_), tools_(other.tools_)
    {
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

    void ChatSession::changeModel(Model model)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        model_ = model;
    }

    void ChatSession::changeSystemInstruction(std::string systemInstruction)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        systemInstruction_ = std::move(systemInstruction);
    }

    void ChatSession::clearHistory()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        history_.clear();
    }

    const std::vector<Content>& ChatSession::history() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return history_;
    }

    std::string ChatSession::getId() const
    {
        return sessionId_;
    }

    nlohmann::json ChatSession::toJson() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        nlohmann::json j;
        j["id"] = sessionId_;
        j["model"] = modelStringRepresentation(model_);
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
        Model m = modelFromStringRepresentation(j["model"]);

        ChatSession session(client, m, j.value("systemInstruction", ""), j.value("id", ""));
        
        if(j.contains("history")) {
            for(const auto& item : j["history"]) {
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
        std::string url;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            
            payload = Internal::PayloadBuilder::build(
                history_, 
                systemInstruction_, 
                {},
                {},
                tools_
            );
            
            url = "https://generativelanguage.googleapis.com/v1beta/models/"
            + std::string(modelStringRepresentation(model_)) + ":generateContent";
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
            return GenerationResult::Failure("Client is null");

        nlohmann::json payload;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            payload = Internal::PayloadBuilder::build(history_, systemInstruction_);
        }
        
        std::string url = "https://generativelanguage.googleapis.com/v1beta/models/"
            + std::string(modelStringRepresentation(model_)) + ":streamGenerateContent?alt=sse";

        GenerationResult result = client_->submitStreamRequest(url, payload, callback);

        if (result.success)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            history_.push_back(result.content);
        }

        return result;
    }
}
