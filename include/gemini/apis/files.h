#pragma once

#ifndef GEMINI_APIS_FILES_H
#define GEMINI_APIS_FILES_H

#include <string>
#include <vector>
#include "gemini/response.h"
#include "gemini/types/files_api_types.h"

namespace GeminiCPP
{
    class Client; // Forward declaration

    class Files
    {
    public:
        explicit Files(Client* client);

        /**
         * @brief Uploads a file to the Gemini API.
         * @param path Local path to the file.
         * @param displayName Optional display name for the file.
         */
        Result<File> upload(const std::string& path, std::string displayName = "");

        /**
         * @brief Gets metadata for a specific file.
         * @param name The resource name of the file (e.g., "files/abc-123").
         */
        Result<File> get(const std::string& name);

        /**
         * @brief Deletes a file.
         * @param name The resource name of the file.
         */
        Result<bool> deleteFile(const std::string& name);

        /**
         * @brief Lists files uploaded by the user.
         * @param pageSize Maximum number of files to return.
         * @param pageToken Token for the next page of results.
         */
        Result<FilesListResponseBody> list(int pageSize = 10, const std::string& pageToken = "");

    private:
        Client* client_;
    };
}

#endif // GEMINI_APIS_FILES_H