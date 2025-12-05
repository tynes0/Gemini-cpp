#pragma once

#ifndef GEMINI_UTILS_H
#define GEMINI_UTILS_H

#include <string>
#include <vector>
#include <filesystem>

namespace GeminiCPP
{
    /**
     * @brief General utility functions for the library.
     */
    class Utils
    {
    public:
        /**
         * @brief Reads a file and converts its content to a Base64 string.
         * @param filepath Path to the file.
         * @return Base64 encoded string of the file content.
         */
        [[nodiscard]] static std::string fileToBase64(const std::string& filepath);
        
        /**
         * @brief Guesses the MIME type based on file extension.
         * @param filepath Path to the file.
         * @return MIME type string (e.g., "image/jpeg"). Defaults to "image/jpeg" if unknown.
         */
        [[nodiscard]] static std::string getMimeType(const std::string& filepath);
        
        /**
         * @brief Decodes a Base64 string into raw bytes.
         */
        [[nodiscard]] static std::vector<unsigned char> base64Decode(const std::string& encoded_string);
        
        /**
         * @brief Maps a MIME type to a common file extension.
         * @param mimeType The MIME type (e.g., "application/pdf").
         * @return File extension including dot (e.g., ".pdf").
         */
        [[nodiscard]] static std::string mimeToExtension(const std::string& mimeType);
        
        /**
         * @brief Parses a JSON error response to extract the user-facing message.
         * @param rawJson The raw JSON response string.
         * @return Extracted error message or the raw JSON if parsing fails.
         */
        [[nodiscard]] static std::string parseErrorMessage(const std::string& rawJson);
        
        /**
         * @brief Encodes raw bytes into a Base64 string.
         */
        [[nodiscard]] static std::string base64Encode(const std::vector<unsigned char>& data);

    };
}
#endif