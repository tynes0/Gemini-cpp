#pragma once

#ifndef GEMINI_STORAGE_H
#define GEMINI_STORAGE_H

#include <string>
#include <vector>
#include <filesystem>

#include "chat_session.h"

namespace GeminiCPP
{
    /**
     * @brief Interface for chat session storage mechanisms.
     * * Allows saving and loading chat sessions to/from different persistence layers
     * (e.g., local disk, database, remote server).
     */
    class IChatStorage
    {
    public:
        virtual ~IChatStorage() = default;

        /**
         * @brief Saves a chat session synchronously.
         */
        virtual bool save(const ChatSession& session) = 0;

        /**
         * @brief Loads a chat session synchronously.
         * @param sessionId The ID of the session to load.
         * @param client Pointer to the Client (needed to reconstruct the Session).
         */
        [[nodiscard]] virtual Result<ChatSession> load(const std::string& sessionId, Client* client) = 0;
        
        /**
         * @brief Lists available session IDs.
         */
        [[nodiscard]] virtual std::vector<std::string> listSessions() = 0;

        /**
        * @brief Saves a chat session asynchronously.
        */
        [[nodiscard]] virtual std::future<bool> saveAsync(const ChatSession& session) = 0;

        /**
        * @brief Loads a chat session asynchronously.
        * @param sessionId The ID of the session to load.
        * @param client Pointer to the Client (needed to reconstruct the Session).
        */
        [[nodiscard]] virtual std::future<Result<ChatSession>> loadAsync(const std::string& sessionId, Client* client) = 0;

        /**
        * @brief Lists available session IDs asynchronously.
        */
        [[nodiscard]] virtual std::future<std::vector<std::string>> listSessionsAsync() = 0;
    };

    /**
     * @brief Implementation of IChatStorage that saves sessions as JSON files on the local disk.
     */
    class LocalStorage : public IChatStorage
    {
    public:
        /**
         * @brief Constructs a LocalStorage.
         * @param rootPath The directory where session files will be stored.
         */
        explicit LocalStorage(std::string rootPath = "chats");

        /**
        * @brief Saves a chat session synchronously.
        */
        bool save(const ChatSession& session) override;

        /**
        * @brief Loads a chat session synchronously.
        * @param sessionId The ID of the session to load.
        * @param client Pointer to the Client (needed to reconstruct the Session).
        */
        [[nodiscard]] Result<ChatSession> load(const std::string& sessionId, Client* client) override;

        /**
        * @brief Lists available session IDs.
        */
        [[nodiscard]] std::vector<std::string> listSessions() override;

        /**
        * @brief Saves a chat session asynchronously.
        */
        [[nodiscard]] std::future<bool> saveAsync(const ChatSession& session) override;

        /**
        * @brief Loads a chat session asynchronously.
        * @param sessionId The ID of the session to load.
        * @param client Pointer to the Client (needed to reconstruct the Session).
        */
        [[nodiscard]] std::future<Result<ChatSession>> loadAsync(const std::string& sessionId, Client* client) override;

        /**
        * @brief Lists available session IDs asynchronously.
        */
        [[nodiscard]] std::future<std::vector<std::string>> listSessionsAsync() override;

    private:
        std::string rootPath_;
    };
    
    /**
     * @brief Implementation of IChatStorage that interacts with a remote server via REST API.
     * @details Assumes standard endpoints: POST /chats (save), GET /chats/{id} (load), GET /chats (list).
     */
    class RemoteStorage : public IChatStorage
    {
    public:
        /**
         * @brief Constructs a RemoteStorage.
         * @param baseUrl The base URL of the remote storage service.
         * @param authToken Optional authorization token (Bearer).
         */
        explicit RemoteStorage(std::string baseUrl, std::string authToken = "");

        /**
        * @brief Saves a chat session synchronously.
        */
        bool save(const ChatSession& session) override;

        /**
        * @brief Loads a chat session synchronously.
        * @param sessionId The ID of the session to load.
        * @param client Pointer to the Client (needed to reconstruct the Session).
        */
        [[nodiscard]] Result<ChatSession> load(const std::string& sessionId, Client* client) override;

        /**
        * @brief Lists available session IDs.
        */
        [[nodiscard]] std::vector<std::string> listSessions() override;

        /**
        * @brief Saves a chat session asynchronously.
        */
        [[nodiscard]] std::future<bool> saveAsync(const ChatSession& session) override;

        /**
        * @brief Loads a chat session asynchronously.
        * @param sessionId The ID of the session to load.
        * @param client Pointer to the Client (needed to reconstruct the Session).
        */
        [[nodiscard]] std::future<Result<ChatSession>> loadAsync(const std::string& sessionId, Client* client) override;

        /**
        * @brief Lists available session IDs asynchronously.
        */
        [[nodiscard]] std::future<std::vector<std::string>> listSessionsAsync() override;

    private:
        std::string baseUrl_;
        std::string authToken_;
    };
}

#endif // GEMINI_STORAGE_H