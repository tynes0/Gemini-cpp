#include "gemini/storage.h"


#include <fstream>
#include <cpr/cpr.h>
#include "gemini/logger.h"
#include "gemini/http_status.h"

namespace GeminiCPP
{
    LocalStorage::LocalStorage(std::string rootPath) : rootPath_(std::move(rootPath))
    {
        if (!std::filesystem::exists(rootPath_)) {
            std::filesystem::create_directories(rootPath_);
        }
    }

    bool LocalStorage::save(const ChatSession& session)
    {
        try {
            std::string filename = session.getId() + ".json";
            std::filesystem::path path = std::filesystem::path(rootPath_) / filename;
                
            std::ofstream file(path);
            if (file.is_open()) {
                file << session.toJson().dump(4);
                return true;
            }
        } catch (const std::exception& e) {
            GEMINI_ERROR("LocalStorage Save Error: {}", e.what());
        }
        return false;
    }

    std::optional<ChatSession> LocalStorage::load(const std::string& sessionId, Client* client)
    {
        try {
            std::filesystem::path path = std::filesystem::path(rootPath_) / (sessionId + ".json");
            if (!std::filesystem::exists(path)) return std::nullopt;

            std::ifstream file(path);
            nlohmann::json j;
            file >> j;
            return ChatSession::fromJson(client, j);
        } catch (...) {
            return std::nullopt;
        }
    }

    std::vector<std::string> LocalStorage::listSessions()
    {
        std::vector<std::string> sessions;
        if (!std::filesystem::exists(rootPath_)) return sessions;

        for (const auto& entry : std::filesystem::directory_iterator(rootPath_)) {
            if (entry.path().extension() == ".json") {
                sessions.push_back(entry.path().stem().string());
            }
        }
        return sessions;
    }

    RemoteStorage::RemoteStorage(std::string baseUrl, std::string authToken)
        : baseUrl_(std::move(baseUrl)), authToken_(std::move(authToken)) 
    {
        if (baseUrl_.back() == '/') baseUrl_.pop_back();
    }

    bool RemoteStorage::save(const ChatSession& session)
    {
        std::string url = baseUrl_ + "/chats";
        nlohmann::json payload = session.toJson();

        cpr::Header headers = {{"Content-Type", "application/json"}};
        if(!authToken_.empty())
            headers.insert({"Authorization", "Bearer " + authToken_});

        url += "/" + session.getId();
            
        cpr::Response r = cpr::Put(
            cpr::Url{url},
            headers,
            cpr::Body{payload.dump()},
            cpr::VerifySsl(false)
        );

        if (HttpStatusHelper::isSuccess(r.status_code)) return true;
            
        GEMINI_ERROR("RemoteStorage Save Error [{}]: {}", r.status_code, r.text);
        return false;
    }

    std::optional<ChatSession> RemoteStorage::load(const std::string& sessionId, Client* client)
    {
        std::string url = baseUrl_ + "/chats/" + sessionId;
            
        cpr::Header headers;
        if(!authToken_.empty())
            headers.insert({"Authorization", "Bearer " + authToken_});

        cpr::Response r = cpr::Get(
            cpr::Url{url},
            headers,
            cpr::VerifySsl(false)
        );

        if (HttpStatusHelper::isSuccess(r.status_code))
        {
            try
            {
                auto j = nlohmann::json::parse(r.text);
                return ChatSession::fromJson(client, j);
            } catch (...)
            {
                GEMINI_ERROR("RemoteStorage JSON Parse Error");
            }
        }
        return std::nullopt;
    }

    std::vector<std::string> RemoteStorage::listSessions()
    {
        std::string url = baseUrl_ + "/chats";
            
        cpr::Header headers;
        
        if(!authToken_.empty())
            headers.insert({"Authorization", "Bearer " + authToken_});

        cpr::Response r = cpr::Get(
            cpr::Url{url},
            headers,
            cpr::VerifySsl(false)
        );

        std::vector<std::string> sessions;
        if (HttpStatusHelper::isSuccess(r.status_code))
        {
            try
            {
                auto j = nlohmann::json::parse(r.text);
                if (j.is_array())
                {
                    for(const auto& item : j)
                        sessions.push_back(item.get<std::string>());
                }
            } catch (...)
            {
                GEMINI_ERROR("RemoteStorage JSON Parse Error");
            }
        }
        return sessions;
    }
}

