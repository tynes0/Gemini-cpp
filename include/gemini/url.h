#pragma once

#ifndef GEMINI_URL_H
#define GEMINI_URL_H

#include <string>
#include <string_view>
#include "model.h"
#include "types.h"

namespace GeminiCPP
{
    enum class ResourceType : uint8_t
    {
        MODEL,          // models/
        FILE,           // files/
        TUNED_MODEL,    // tunedModels/
        CORPUS,         // corpora/
        OPERATION,      // operations/
        CACHED_CONTENT, // cachedContents/
        NONE            // No Prefix (Raw string)
    };
    
    enum class EndpointType : uint8_t
    {
        REST, // Standard API (generativelanguage.googleapis.com/v1beta/)
        UPLOAD // File Upload (generativelanguage.googleapis.com/upload/v1beta/)
    };
    
    // --- GENERAL RESOURCE MANAGER ---
    // ResourceName("gemini-2.5", ResourceType::MODEL) -> "models/gemini-1.5"
    // ResourceName("video.mp4", ResourceType::FILE)   -> "files/video.mp4"
    class ResourceName
    {
    public:
        ResourceName(std::string name, ResourceType type = ResourceType::MODEL);
        ResourceName(std::string_view name, ResourceType type = ResourceType::MODEL);
        ResourceName(const char* name, ResourceType type = ResourceType::MODEL);
        
        ResourceName(GeminiCPP::Model model);

        // -- Static Factory Methods --
        // Ex: ResourceName::File("video123")
        static ResourceName File(std::string_view name);
        static ResourceName Model(std::string_view name);
        static ResourceName TunedModel(std::string_view name);
        static ResourceName Corpus(std::string_view name);
        static ResourceName Operation(std::string_view name);
        static ResourceName CachedContent(std::string_view name);
        static ResourceName Raw(std::string_view name); // No Prefix

        ResourceName& operator=(const std::string& name);
        
        [[nodiscard]] std::string str() const;
        operator std::string() const;

    private:
        std::string value_;
        ResourceType type_;
        
        void ensurePrefix();
    };

    // --- URL BUILDER ---
    class Url
    {
    public:
        static constexpr std::string_view BASE_URL_REST = "https://generativelanguage.googleapis.com/v1beta/";
        static constexpr std::string_view BASE_URL_UPLOAD = "https://generativelanguage.googleapis.com/upload/v1beta/";

        Url() = default;
        Url(const Url&) = default;
        Url& operator=(const Url&) = default;
        Url(Url&&) = default;
        Url& operator=(Url&&) = default;
        ~Url() = default;

        Url(const ResourceName& resource, std::string_view action);
        Url(const ResourceName& resource, GenerationMethod action);

        explicit Url(const ResourceName& resource);

        explicit Url(std::string_view endpoint, EndpointType type = EndpointType::REST);
        
        Url& addQuery(std::string_view key, std::string_view value = "");

        [[nodiscard]] std::string str() const;
        [[nodiscard]] operator std::string() const;

    private:
        // Helper: Returns the base URL based on the type
        static std::string_view getBase(EndpointType type);
        
        std::string full_url_;
        bool has_query_ = false;
    };
}

#endif // GEMINI_URL_H