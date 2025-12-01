#include "gemini/apis/files.h"
#include "gemini/client.h"
#include "gemini/utils.h"

namespace GeminiCPP
{
    Files::Files(Client* client)
        : client_(client)
    {
    }

    Result<File> Files::upload(const std::string& path, std::string displayName)
    {
        if (!std::filesystem::exists(path))
        {
            return Result<File>::Failure("File not found: " + path);
        }

        std::string mimeType = Utils::getMimeType(path);
        if (displayName.empty())
        {
            displayName = std::filesystem::path(path).filename().string();
        }

        // Metadata JSON
        nlohmann::json metadata = {
            {"file", {
                {"display_name", displayName}
                }
            }
        };

        
        return client_->postMultipart<File>("files", path, mimeType, metadata);
    }

    Result<File> Files::get(const std::string& name)
    {
        return client_->get<File>(name);
    }

    Result<bool> Files::deleteFile(const std::string& name)
    {
        return client_->deleteResource(name);
    }

    Result<FilesListResponseBody> Files::list(int pageSize, const std::string& pageToken)
    {
        std::map<std::string, std::string> params;
        params["pageSize"] = std::to_string(pageSize);
        if (!pageToken.empty())
            params["pageToken"] = pageToken;

        return client_->get<FilesListResponseBody>("files", params);
    }
}