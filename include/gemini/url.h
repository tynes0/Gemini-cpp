#pragma once

#ifndef GEMINI_URL_H
#define GEMINI_URL_H

#include <string>
#include <string_view>
#include "generation_method.h"

namespace GeminiCPP
{
    /**
     * @brief Enum defining different types of Google API resources.
     */
    FrenumClassInNamespace(GeminiCPP, ResourceType, uint8_t,
        MODEL,          // models/
        FILE,           // files/
        OPERATION,      // operations/
        CACHED_CONTENT, // cachedContents/
        BATCH,          // batches/
        FILE_SEARCH_STORES, // fileSearchStores/
        NONE            // No Prefix (Raw string)
    )
    
    /**
     * @brief Enum defining endpoint types (Standard REST or Upload).
     */
    FrenumClassInNamespace(GeminiCPP, EndpointType, uint8_t,
        REST, // Standard API (generativelanguage.googleapis.com/v1beta/)
        UPLOAD // File Upload (generativelanguage.googleapis.com/upload/v1beta/)
    )
    
    /**
     * @brief Helper class to manage and format resource names.
     * * Ensures correct prefixes (e.g., "models/", "files/") are applied.
     */
    class ResourceName
    {
    public:
        ResourceName() = default;
        ResourceName(const ResourceName&) = default;
        ResourceName& operator=(const ResourceName&) = default;
        ResourceName(ResourceName&& other) noexcept = default;
        ResourceName& operator=(ResourceName&& other) noexcept = default;
        ~ResourceName() = default;
        
        /**
         * @brief Constructs a ResourceName with an explicit type.
         */
        explicit ResourceName(std::string name, ResourceType type = ResourceType::MODEL);
        ResourceName& operator=(const std::string& name);
        
        // -- Static Factory Methods --
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

    /**
     * @brief Helper class to build full API URLs.
     */
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

        /**
         * @brief Constructs a URL for a resource and an action string.
         * @param resource The target resource.
         * @param action The action suffix (e.g., ":generateContent").
         */
        Url(const ResourceName& resource, const std::string& action);

        /**
         * @brief Constructs a URL for a resource and a GenerationMethod enum.
         */
        Url(const ResourceName& resource, GenerationMethod action);

        /**
         * @brief Constructs a URL for a resource (usually for GET requests).
         */
        explicit Url(const ResourceName& resource);

        /**
         * @brief Constructs a URL from a raw endpoint string.
         */
        explicit Url(const std::string& endpoint, EndpointType type = EndpointType::REST);
        
        /**
         * @brief Appends a query parameter to the URL.
         */
        Url& addQuery(const std::string& key, const std::string& value = "");

        [[nodiscard]] std::string str() const;
        [[nodiscard]] explicit operator std::string() const;

    private:
        static std::string_view getBase(EndpointType type);
        
        std::string full_url_;
        bool has_query_ = false;
    };
}

#endif // GEMINI_URL_H