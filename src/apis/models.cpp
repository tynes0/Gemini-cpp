#include "gemini/apis/models.h"
#include "gemini/client.h"

namespace GeminiCPP
{
    Models::Models(Client* client)
        : client_(client)
    {
    }

    Result<ModelInfo> Models::get(const std::string& name)
    {
        return client_->get<ModelInfo>(name);
    }

    Result<ModelsListResponseBody> Models::list(int pageSize, const std::string& pageToken)
    {
        std::map<std::string, std::string> params;
        params["pageSize"] = std::to_string(pageSize);
        if (!pageToken.empty())
        {
            params["pageToken"] = pageToken;
        }

        return client_->get<ModelsListResponseBody>("models", params);
    }
}