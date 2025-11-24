#pragma once

#ifndef GEMINI_UTILS_H
#define GEMINI_UTILS_H

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "logger.h"

namespace GeminiCPP
{
    class Utils
    {
    public:
        [[nodiscard]] static std::string fileToBase64(const std::string& filepath);
        [[nodiscard]] static std::string getMimeType(const std::string& filepath);
        [[nodiscard]] static std::vector<unsigned char> base64Decode(const std::string& encoded_string);
        [[nodiscard]] static std::string mimeToExtension(const std::string& mimeType);
        [[nodiscard]] static std::string parseErrorMessage(const std::string& rawJson);

    private:
        [[nodiscard]] static std::string base64Encode(const std::vector<unsigned char>& data);
    };
}
#endif