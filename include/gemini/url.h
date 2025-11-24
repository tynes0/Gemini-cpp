#pragma once

#ifndef GEMINI_URL_H
#define GEMINI_URL_H

#include <string>
#include <string_view>
#include "model.h" // Model enum desteği için

namespace GeminiCPP
{
    // --- MODEL NAME HELPER ---
    class ModelName
    {
    public:
        ModelName(std::string name);
        ModelName(std::string_view name);
        ModelName(const char* name);
        ModelName(Model model);

        ModelName& operator=(const std::string& name);
        ModelName& operator=(std::string_view name);
        ModelName& operator=(Model model);

        [[nodiscard]] std::string str() const;
        operator std::string() const;

    private:
        std::string value_;
        void ensurePrefix();
    };

    // --- URL BUILDER ---
    // https://generativelanguage.googleapis.com/v1beta/ based
    class Url
    {
    public:
        static constexpr std::string_view BASE_URL = "https://generativelanguage.googleapis.com/v1beta/";

        Url() = default;
        Url(const Url&) = default;
        Url& operator=(const Url&) = default;
        Url(Url&&) = default;
        Url& operator=(Url&&) = default;
        ~Url() = default;
        
        Url(const ModelName& model, std::string_view action);

        explicit Url(const ModelName& model);
        explicit Url(std::string_view endpoint);
        
        Url& addQuery(std::string_view key, std::string_view value = "");

        [[nodiscard]] std::string str() const;
        operator std::string() const;

    private:
        std::string full_url_;
        bool has_query_ = false;
    };
}

#endif // GEMINI_URL_H