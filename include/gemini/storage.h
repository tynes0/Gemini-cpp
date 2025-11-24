#pragma once

#ifndef GEMINI_STORAGE_H
#define GEMINI_STORAGE_H

#include <string>
#include <vector>
#include <filesystem>
#include <optional>

#include "chat_session.h"

namespace GeminiCPP
{
    class IChatStorage
    {
    public:
        virtual ~IChatStorage() = default;

        virtual bool save(const ChatSession& session) = 0;
        [[nodiscard]] virtual Result<ChatSession> load(const std::string& sessionId, Client* client) = 0;
        [[nodiscard]] virtual std::vector<std::string> listSessions() = 0;
    };

    class LocalStorage : public IChatStorage
    {
    public:
        explicit LocalStorage(std::string rootPath = "chats");

        bool save(const ChatSession& session) override;
        [[nodiscard]] Result<ChatSession> load(const std::string& sessionId, Client* client) override;
        [[nodiscard]] std::vector<std::string> listSessions() override;

    private:
        std::string rootPath_;
    };

    // --- (REMOTE SERVER) ---
    // We assume the server API works like this:
    // POST /chats        -> Save (Body: Chat JSON)
    // GET  /chats/{id}   -> Load
    // GET  /chats        -> List (Return: ["id1", "id2"])
    class RemoteStorage : public IChatStorage
    {
    public:
        explicit RemoteStorage(std::string baseUrl, std::string authToken = "");

        bool save(const ChatSession& session) override;
        [[nodiscard]] Result<ChatSession> load(const std::string& sessionId, Client* client) override;
        [[nodiscard]] std::vector<std::string> listSessions() override;

    private:
        std::string baseUrl_;
        std::string authToken_;
    };
}

#endif // GEMINI_STORAGE_H