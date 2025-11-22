#pragma once

#ifndef GEMINI_CLIENT_HPP
#define GEMINI_CLIENT_HPP

#include <string>
#include <string_view>
#include "model.h"

namespace GeminiCPP
{
    class Client
    {
    public:
        explicit Client(std::string api_key);

        std::string generateContent(const std::string& prompt, std::string_view model_id);
        std::string generateContent(const std::string& prompt, Model model = Model::GEMINI_2_5_FLASH);
    private:
        std::string api_key_;
        const std::string root_url_ = "https://generativelanguage.googleapis.com/v1beta/models/";
    };

}

#endif // GEMINI_CLIENT_HPP