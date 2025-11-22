#include "gemini/chat_session.h"
#include "gemini/client.h"
#include <nlohmann/json.hpp>

#include "internal/payload_builder.h"

namespace GeminiCPP
{
    ChatSession::ChatSession(Client* client, Model model, std::string systemInstruction)
            : client_(client), model_(model), systemInstruction_(std::move(systemInstruction))
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

    void ChatSession::changeModel(Model model)
    {
        model_ = model;
    }

    void ChatSession::changeSystemInstruction(std::string systemInstruction)
    {
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
            payload = Internal::PayloadBuilder::build(history_, systemInstruction_);
            
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
}
