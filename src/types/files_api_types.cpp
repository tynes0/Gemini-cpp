#include "gemini/types/files_api_types.h"

#include "nlohmann/json.hpp"

namespace GeminiCPP
{
    File File::fromJson(const nlohmann::json& j)
    {
        File f{};
        
        if(j.contains("name")) f.name = j.value("name", "");
        if(j.contains("displayName")) f.displayName = j.value("displayName", "");
        if(j.contains("mimeType")) f.mimeType = j.value("mimeType", "");
        if(j.contains("sizeBytes")) f.sizeBytes = j.value("sizeBytes", "0");
        if(j.contains("createTime")) f.createTime = j.value("createTime", "");
        if(j.contains("updateTime")) f.updateTime = j.value("updateTime", "");
        if(j.contains("expirationTime")) f.expirationTime = j.value("expirationTime", "");
        if(j.contains("sha256Hash")) f.sha256Hash = j.value("sha256Hash", "");
        if(j.contains("uri")) f.uri = j.value("uri", "");
            
        if(j.contains("state"))
        {
            auto s = frenum::cast<FileState>(j.value("state", ""));
            if(s.has_value()) f.state = s.value();
        }
        
        if(j.contains("error") && j["error"].contains("message"))
        {
            f.errorMsg = j["error"]["message"].get<std::string>();
        }

        return f;
    }

    std::string File::toString() const
    {
        return "File: " + displayName + " (" + name + ")\n" +
               "URI: " + uri + "\n" +
               "State: " + frenum::to_string(state) + "\n" +
               "MIME: " + mimeType + " (" + sizeBytes + " bytes)";
    }

    ListFilesResponse ListFilesResponse::fromJson(const nlohmann::json& j)
    
    {
        ListFilesResponse r;
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