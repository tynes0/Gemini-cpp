#include "gemini/types/files_api_types.h"

#include "gemini/logger.h"
#include "nlohmann/json.hpp"

namespace GeminiCPP
{
    Status Status::fromJson(const nlohmann::json& j)
    {
        Status result{};

        result.code = static_cast<HttpMappedStatusCode>(j.value("code", 0));
        result.message = j.value("message", "");
        if (j.contains("details"))
            result.details = j.get<std::vector<nlohmann::json>>();

        return result;
    }

    nlohmann::json Status::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["code"] = frenum::value(code);
        j["message"] = message;
        j["details"] = details;
        
        return j;
    }

    VideoFileMetadata VideoFileMetadata::fromJson(const nlohmann::json& j)
    {
        VideoFileMetadata result{};

        result.videoDuration = j.value("videoDuration", "");

        return result;
    }

    nlohmann::json VideoFileMetadata::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["videoDuration"] = videoDuration.str();
        
        return j;
    }

    File File::fromJson(const nlohmann::json& j)
    {
        File result{};

        result.name = j.value("name", "");
        if (j.contains("displayName"))
            result.displayName = j["displayName"].get<std::string>();

        try
        {
            std::string sizeStr = j.value("sizeBytes", "0");
            result.sizeBytes = std::stoi(sizeStr);
        }
        catch (const std::exception& e)
        {
            GEMINI_ERROR("sizeBytes string to int64_t casting failed. ({})", e.what());
        }

        result.createTime = j.value("createTime", "");
        result.updateTime = j.value("updateTime", "");
        result.expirationTime = j.value("expirationTime", "");
        result.sha256Hash = Support::Base64String::fromBase64(j.value("sha256Hash", ""));
        result.uri = j.value("uri", "");
        result.downloadUri = j.value("downloadUri", "");
        result.state = frenum::cast<FileState>(j.value("state", "")).value_or(FileState::STATE_UNSPECIFIED);
        result.source = frenum::cast<Source>(j.value("source", "")).value_or(Source::SOURCE_UNSPECIFIED);
        if (j.contains("error"))
            result.error = Status::fromJson(j["error"]);
        if (j.contains("metadata"))
            result.metadata = VideoFileMetadata::fromJson(j["videoMetadata"]);

        return result;
    }

    nlohmann::json File::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["name"] = name.str();
        if (displayName.has_value())
            j["displayName"] = *displayName;
        j["mimeType"] = mimeType;
        j["sizeBytes"] = std::to_string(sizeBytes);
        j["createTime"] = createTime.str();
        j["updateTime"] = updateTime.str();
        j["expirationTime"] = expirationTime.str();
        j["sha256Hash"] = sha256Hash.str();
        j["uri"] = uri;
        j["downloadUri"] = downloadUri;
        j["state"] = frenum::to_string(state);
        j["source"] = frenum::to_string(source);
        j["error"] = error.toJson();

        std::visit([&j]<typename T>(const T& arg)
        {
            if constexpr (std::is_same_v<T, VideoFileMetadata>)
                j["videoMetadata"] = arg.toJson();
        }, metadata);
        
        return j;
    }

    MediaUploadResponseBody MediaUploadResponseBody::fromJson(const nlohmann::json& j)
    {
        MediaUploadResponseBody result{};

        if (j.contains("file") && j["file"].is_object())
            result.file = File::fromJson(j["file"]);
        
        return result;
    }

    nlohmann::json MediaUploadResponseBody::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["file"] = file.toJson();
        
        return j;
    }

    FilesGetRequestBody FilesGetRequestBody::fromJson(const nlohmann::json& j)
    {
        return FilesGetRequestBody{};
    }

    nlohmann::json FilesGetRequestBody::toJson() const
    {
        return nlohmann::json::object();
    }

    FilesGetResponseBody FilesGetResponseBody::fromJson(const nlohmann::json& j)
    {
        FilesGetResponseBody result{};
        result.response = File::fromJson(j);
        return result;
    }

    nlohmann::json FilesGetResponseBody::toJson() const
    {
        return response.toJson();
    }

    FilesListQueryParameters FilesListQueryParameters::fromJson(const nlohmann::json& j)
    {
        FilesListQueryParameters result{};

        if (j.contains("pageSize"))
            result.pageSize = j["pageSize"];
        if (j.contains("pageToken"))
            result.pageToken = j["pageToken"];

        return result;
    }

    nlohmann::json FilesListQueryParameters::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (pageSize.has_value())
            j["pageSize"] = pageSize.value();
        if (pageToken.has_value())
            j["pageToken"] = pageToken.value();
        
        return j;
    }

    FilesListRequestBody FilesListRequestBody::fromJson(const nlohmann::json& j)
    {
        return FilesListRequestBody{};
    }

    nlohmann::json FilesListRequestBody::toJson() const
    {
        return nlohmann::json::object();
    }

    FilesListResponseBody FilesListResponseBody::fromJson(const nlohmann::json& j)
    
    {
        FilesListResponseBody result;
        if(j.contains("files") && j["files"].is_array())
        {
            for(const auto& item : j["files"])
            {
                result.files.push_back(File::fromJson(item));
            }
        }
        if(j.contains("nextPageToken")) result.nextPageToken = j.value("nextPageToken", "");
        return result;
    }

    nlohmann::json FilesListResponseBody::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        j["files"] = nlohmann::json::array();
        for(const auto& file : files)
            j["files"].push_back(file.toJson());

        j["nextPageToken"] = nextPageToken;
        
        return j;
    }

    FilesDeleteRequestBody FilesDeleteRequestBody::fromJson(const nlohmann::json& j)
    {
        return FilesDeleteRequestBody{};
    }

    nlohmann::json FilesDeleteRequestBody::toJson() const
    {
        return nlohmann::json::object();
    }

    FilesDeleteResponseBody FilesDeleteResponseBody::fromJson(const nlohmann::json& j)
    {
        return FilesDeleteResponseBody{};
    }

    nlohmann::json FilesDeleteResponseBody::toJson() const
    {
        return nlohmann::json::object();
    }

    MediaUploadRequestBody MediaUploadRequestBody::fromJson(const nlohmann::json& j)
    {
        MediaUploadRequestBody result{};

        if (j.contains("file") && j["file"].is_object())
            result.file = File::fromJson(j["file"]);
        
        return result;
    }

    nlohmann::json MediaUploadRequestBody::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (file.has_value())
            j["file"] = file->toJson();
        
        return j;
    }
}
