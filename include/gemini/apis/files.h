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

    /**
     * @brief Provides methods to interact with the Files API.
     * * Allows uploading media files (images, videos, etc.) to Google servers for use in
     * subsequent generation requests (e.g., analyzing a video or an image).
     */
    class Files
    {
    public:
        /**
         * @brief Constructs a new Files API module.
         * @param client Pointer to the main Client instance.
         */
        explicit Files(Client* client);

        /**
         * @brief Uploads a file to the Gemini API.
         * @details The file is uploaded using the multipart/related protocol.
         * @param path Local path to the file to upload.
         * @param displayName Optional custom display name for the file. If empty, filename is used.
         * @return Result<File> containing metadata of the uploaded file.
         */
        Result<File> upload(const std::string& path, std::string displayName = "");

        /**
         * @brief Retrieves metadata for a specific file.
         * @param name The resource name of the file (e.g., "files/abc-123").
         * @return Result<File> containing the file metadata.
         */
        Result<File> get(const std::string& name);

        /**
         * @brief Deletes a file from the server.
         * @param name The resource name of the file (e.g., "files/abc-123").
         * @return Result<bool> true if deletion was successful.
         */
        Result<bool> deleteFile(const std::string& name);

        /**
         * @brief Lists files uploaded by the project.
         * @param pageSize Maximum number of files to return (default 10).
         * @param pageToken Token for the next page of results (for pagination).
         * @return Result<FilesListResponseBody> containing the list of files and next page token.
         */
        Result<FilesListResponseBody> list(int pageSize = 10, const std::string& pageToken = "");

    private:
        Client* client_;
    };
}

#endif // GEMINI_APIS_FILES_H