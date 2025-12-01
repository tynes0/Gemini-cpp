#pragma once

#ifndef GEMINI_URL_H
#define GEMINI_URL_H

#include <string>
#include <string_view>
#include "model.h"

namespace GeminiCPP
{
    FrenumClassInNamespace(GeminiCPP, ResourceType, uint8_t,
        MODEL,          // models/
        FILE,           // files/
        OPERATION,      // operations/
        CACHED_CONTENT, // cachedContents/
        BATCH,          // batches/
        FILE_SEARCH_STORES, // fileSearchStores/
        NONE            // No Prefix (Raw string)
    )
    
    FrenumClassInNamespace(GeminiCPP, EndpointType, uint8_t,
        REST, // Standard API (generativelanguage.googleapis.com/v1beta/)
        UPLOAD // File Upload (generativelanguage.googleapis.com/upload/v1beta/)
    )
    
    // --- GENERAL RESOURCE MANAGER ---
    // ResourceName("gemini-2.5", ResourceType::MODEL) -> "models/gemini-1.5"
    // ResourceName("video.mp4", ResourceType::FILE)   -> "files/video.mp4"
    class ResourceName
    {
    public:
        ResourceName() = default;
        ResourceName(const ResourceName&) = default;
        ResourceName& operator=(const ResourceName&) = default;
        ResourceName(ResourceName&& other) noexcept = default;
        ResourceName& operator=(ResourceName&& other) noexcept = default;
        ~ResourceName() = default;
        
        explicit ResourceName(std::string name, ResourceType type = ResourceType::MODEL);
        ResourceName& operator=(const std::string& name);
        
        ResourceName(Model model);

        // -- Static Factory Methods --
        // Ex: ResourceName::File("video123")
        static ResourceName File(std::string name);
        static ResourceName Model(std::string name);
        static ResourceName Operation(std::string name);
        static ResourceName CachedContent(std::string name);
        static ResourceName Batch(std::string name);
        static ResourceName FileSearchStores(std::string name);
        static ResourceName Raw(std::string name); // No Prefix
        
        [[nodiscard]] std::string str() const;
        explicit operator std::string() const;

    private:
        std::string value_;
        ResourceType type_ = ResourceType::NONE;

        void ensurePrefix();
        static ResourceType detectTypeFromPrefix(std::string_view s);
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
        [[nodiscard]] explicit operator std::string() const;

    private:
        // Helper: Returns the base URL based on the type
        static std::string_view getBase(EndpointType type);
        
        std::string full_url_;
        bool has_query_ = false;
    };
}

#endif // GEMINI_URL_H