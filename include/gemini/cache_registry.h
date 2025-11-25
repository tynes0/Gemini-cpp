#pragma once

#ifndef GEMINI_CACHE_REGISTRY_H
#define GEMINI_CACHE_REGISTRY_H

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "types.h"

namespace GeminiCPP
{
    struct CachedItemInfo
    {
        std::string id;           // cachedContents/xyz
        std::string alias;        // Name given by the user (e.g. "HolidayVideo")
        std::string model;        // For which model?
        std::string expireTime;   // When will it die?

        [[nodiscard]] nlohmann::json toJson() const;
        [[nodiscard]] static CachedItemInfo fromJson(const nlohmann::json& j);
    };

    class CacheRegistry
    {
    public:
        explicit CacheRegistry(std::string registryPath = "gemini_caches.json");

        void registerCache(const std::string& alias, const CachedContent& content);
        [[nodiscard]] std::optional<std::string> getCacheId(const std::string& alias) const;
        [[nodiscard]] std::optional<CachedItemInfo> getCacheInfo(const std::string& alias) const;
        void unregisterCache(const std::string& alias);
        [[nodiscard]] std::vector<std::string> listAliases() const;

    private:
        std::string registryPath_;
        nlohmann::json registry_;

        void load();
        void save() const;
    };
}

#endif // GEMINI_CACHE_REGISTRY_H