#include "gemini/types/files_api_types.h"

#include "nlohmann/json.hpp"

namespace GeminiCPP
{
    File File::fromJson(const nlohmann::json& j)
    {
        File f{};
        

        return f;
    }

    FilesListResponseBody FilesListResponseBody::fromJson(const nlohmann::json& j)
    
    {
        FilesListResponseBody r;
        if(j.contains("files") && j["files"].is_array())
        {
            for(const auto& item : j["files"])
            {
                r.files.push_back(File::fromJson(item));
            }
        }
        if(j.contains("nextPageToken")) r.nextPageToken = j.value("nextPageToken", "");
        return r;
    }
}