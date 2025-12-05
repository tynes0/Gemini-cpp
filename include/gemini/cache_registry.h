#pragma once

#ifndef GEMINI_CACHE_REGISTRY_H
#define GEMINI_CACHE_REGISTRY_H

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "types/caching_api_types.h"

namespace GeminiCPP
{
    /**
     * @brief Stores metadata about a cached content item.
     * * This structure is used by the CacheRegistry to track locally known caches.
     */
    struct CachedItemInfo
    {
        std::string id;           ///< The unique resource ID (e.g., "cachedContents/xyz").
        std::string alias;        ///< User-defined friendly name (e.g., "HolidayVideo").
        std::string model;        ///< The model this cache is compatible with.
        std::string expireTime;   ///< Expiration timestamp.

        [[nodiscard]] nlohmann::json toJson() const;
        [[nodiscard]] static CachedItemInfo fromJson(const nlohmann::json& j);
    };

    /**
     * @brief Manages a local registry of cached content.
     * * Allows developers to map friendly names (aliases) to complex server-side IDs.
     * * The registry is persisted to a JSON file (default: "gemini_caches.json").
     */
    class CacheRegistry
    {
    public:
        /**
         * @brief Constructs a CacheRegistry.
         * @param registryPath Path to the JSON file where the registry is stored.
         */
        explicit CacheRegistry(std::string registryPath = "gemini_caches.json");

        /**
         * @brief Registers a new cache entry.
         * @param alias A friendly name for the cache.
         * @param content The CachedContent object returned from the API.
         */
        void registerCache(const std::string& alias, const CachedContent& content);
        
        /**
         * @brief Retrieves the server ID associated with an alias.
         * @param alias The friendly name.
         * @return std::optional<std::string> The resource ID if found.
         */
        [[nodiscard]] std::optional<std::string> getCacheId(const std::string& alias) const;
        
        /**
         * @brief Retrieves full metadata for a cached item.
         */
        [[nodiscard]] std::optional<CachedItemInfo> getCacheInfo(const std::string& alias) const;
        
        /**
         * @brief Removes a cache entry from the local registry.
         * @note This does NOT delete the cache from the server.
         */
        void unregisterCache(const std::string& alias);
        
        /**
         * @brief Lists all registered aliases.
         */
        [[nodiscard]] std::vector<std::string> listAliases() const;

    private:
        std::string registryPath_;
        nlohmann::json registry_;

        void load();
        void save() const;
    };
}

#endif // GEMINI_CACHE_REGISTRY_H