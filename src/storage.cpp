#include "gemini/storage.h"

#include <fstream>
#include <cpr/cpr.h>
#include "gemini/logger.h"
#include "gemini/http_mapped_status_code.h"
#include "gemini/utils.h"

using namespace std::string_literals;

namespace GeminiCPP
{
    LocalStorage::LocalStorage(std::string rootPath)
        : rootPath_(std::move(rootPath))
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
            if (file.is_open())
            {
                file << session.toJson().dump(4);
                return true;
            }
        }
        catch (const std::exception& e)
        {
            GEMINI_ERROR("LocalStorage Save Error: {}", e.what());
        }
        return false;
    }

    Result<ChatSession> LocalStorage::load(const std::string& sessionId, Client* client)
    {
        try
        {
            std::filesystem::path path = std::filesystem::path(rootPath_) / (sessionId + ".json");
            if (!std::filesystem::exists(path))
                return Result<ChatSession>::Failure("Session file not found: " + path.string(), frenum::value(HttpMappedStatusCode::OK));

            std::ifstream file(path);
            nlohmann::json j;
            file >> j;
            return Result<ChatSession>::Success(ChatSession::fromJson(client, j));
        }
        catch (const std::exception& e)
        {
            return Result<ChatSession>::Failure("Load Error: "s + e.what());
        }
    }

    std::vector<std::string> LocalStorage::listSessions()
    {
        std::vector<std::string> sessions;
        if (!std::filesystem::exists(rootPath_)) return sessions;

        for (const auto& entry : std::filesystem::directory_iterator(rootPath_))
        {
            if (entry.path().extension() == ".json")
            {
                sessions.push_back(entry.path().stem().string());
            }
        }
        return sessions;
    }

    std::future<bool> LocalStorage::saveAsync(const ChatSession& session)
    {
        // Copying the session can be expensive, so instead of a const reference,
        // it might make more sense to retrieve and pass the JSON output.
        // However, since the session.toJson() call is thread-safe,
        // we can call it here and pass the string data to the lambda.
        
        nlohmann::json data = session.toJson();
        std::string id = session.getId();

        return std::async(std::launch::async, [this, id, data = std::move(data)]() {
            try
            {
                std::string filename = id + ".json";
                std::filesystem::path path = std::filesystem::path(rootPath_) / filename;
                std::ofstream file(path);
                if (file.is_open()) {
                    file << data.dump(4);
                    return true;
                }
            } catch (const std::exception& e)
            {
                GEMINI_ERROR("LocalStorage Save Error: {}", e.what());
            }
            return false;
        });
    }

    std::future<Result<ChatSession>> LocalStorage::loadAsync(const std::string& sessionId, Client* client)
    {
        return std::async(std::launch::async, [this, sessionId, client = client]() {
            return load(sessionId, client);
        });
    }

    std::future<std::vector<std::string>> LocalStorage::listSessionsAsync()
    {
        return std::async(std::launch::async, [this]() {
            return listSessions();
        });
    }

    std::future<bool> RemoteStorage::saveAsync(const ChatSession& session)
    {
        nlohmann::json payload = session.toJson();
        std::string id = session.getId();

        return std::async(std::launch::async, [this, id, payload = std::move(payload)]() {
            std::string url = baseUrl_ + "/chats/" + id;

            cpr::Header headers = {{"Content-Type", "application/json"}};
            if (!authToken_.empty()) {
                headers.insert({"Authorization", "Bearer " + authToken_});
            }

            cpr::Response r = cpr::Put(
                cpr::Url{url},
                headers,
                cpr::Body{payload.dump()},
                cpr::VerifySsl(false)
            );

            if (HttpMappedStatusCodeHelper::isSuccess(r.status_code)) {
                return true;
            }

            GEMINI_ERROR("RemoteStorage Async Save Error [{}]: {}", r.status_code, Utils::parseErrorMessage(r.text));
            return false;
        });
    }

    std::future<Result<ChatSession>> RemoteStorage::loadAsync(const std::string& sessionId, Client* client)
    {
        return std::async(std::launch::async, [this, sessionId, client]() {
            return load(sessionId, client);
        });
    }

    std::future<std::vector<std::string>> RemoteStorage::listSessionsAsync()
    {
        return std::async(std::launch::async, [this]() {
            return listSessions();
        });
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

        if (HttpMappedStatusCodeHelper::isSuccess(r.status_code))
            return true;
            
        GEMINI_ERROR("RemoteStorage Save Error [{}]: {}", r.status_code, Utils::parseErrorMessage(r.text));
        return false;
    }

    Result<ChatSession> RemoteStorage::load(const std::string& sessionId, Client* client)
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

        if (HttpMappedStatusCodeHelper::isSuccess(r.status_code))
        {
            try
            {
                auto j = nlohmann::json::parse(r.text);
                return Result<ChatSession>::Success(ChatSession::fromJson(client, j));
            }
            catch (const std::exception& e)
            {
                return Result<ChatSession>::Failure(std::string("Load Error: ") + e.what());
            }
        }

        std::string errorMsg = Utils::parseErrorMessage(r.text);
        GEMINI_ERROR("ListModels Error [{}]: {}", r.status_code, errorMsg);
        return Result<ChatSession>::Failure(errorMsg, r.status_code);
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
        if (HttpMappedStatusCodeHelper::isSuccess(r.status_code))
        {
            try
            {
                auto j = nlohmann::json::parse(r.text);
                if (j.is_array())
                {
                    for(const auto& item : j)
                        sessions.push_back(item.get<std::string>());
                }
            }
            catch (const std::exception& e)
            {
                GEMINI_ERROR("RemoteStorage JSON Parse Error: {}", e.what());
            }
        }
        return sessions;
    }
}

