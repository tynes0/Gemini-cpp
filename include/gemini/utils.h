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
        static std::string fileToBase64(const std::string& filepath);
        static std::string getMimeType(const std::string& filepath);
        static std::vector<unsigned char> base64Decode(const std::string& encoded_string);

    private:
        static std::string base64Encode(const std::vector<unsigned char>& data);
    };
}
#endif