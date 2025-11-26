#include "gemini/cache_registry.h"

#include "gemini/logger.h"

namespace GeminiCPP
{
    nlohmann::json CachedItemInfo::toJson() const
    {
        return {
                    {"id", id},
                    {"alias", alias},
                    {"model", model},
                    {"expireTime", expireTime}
        };
    }

    CachedItemInfo CachedItemInfo::fromJson(const nlohmann::json& j)
    {
        return {
            j.value("id", ""),
            j.value("alias", ""),
            j.value("model", ""),
            j.value("expireTime", "")
        };
    }

    CacheRegistry::CacheRegistry(std::string registryPath)
            : registryPath_(std::move(registryPath))
    {
        load();
    }

    void CacheRegistry::registerCache(const std::string& alias, const CachedContent& content)
    
    {
        CachedItemInfo info;
        info.id = content.name;
        info.alias = alias;
        info.model = content.model;
        info.expireTime = content.expireTime;

        registry_[alias] = info.toJson();
        save();
    }

    std::optional<std::string> CacheRegistry::getCacheId(const std::string& alias) const
    {
        if (registry_.contains(alias))
        {
            return registry_[alias]["id"].get<std::string>();
        }
        return std::nullopt;
    }

    std::optional<CachedItemInfo> CacheRegistry::getCacheInfo(const std::string& alias) const
    {
        if (registry_.contains(alias))
        {
            return CachedItemInfo::fromJson(registry_[alias]);
        }
        return std::nullopt;
    }

    void CacheRegistry::unregisterCache(const std::string& alias)
    {
        if (registry_.contains(alias))
        {
            registry_.erase(alias);
            save();
        }
    }

    std::vector<std::string> CacheRegistry::listAliases() const
    {
        std::vector<std::string> aliases;
        for (auto& [key, val] : registry_.items())
        {
            aliases.push_back(key);
        }
        return aliases;
    }

    void CacheRegistry::load()
    {
        if (std::filesystem::exists(registryPath_))
        {
            try
            {
                std::ifstream file(registryPath_);
                file >> registry_;
            }
            catch (const std::exception& e)
            {
                GEMINI_WARN("Failed to load cache registry ({}). Starting fresh.", e.what());
                registry_ = nlohmann::json::object();
            }
        } else
        {
            registry_ = nlohmann::json::object();
        }
    }

    void CacheRegistry::save() const
    {
        try
        {
            std::ofstream file(registryPath_);
            file << registry_.dump(4);
        }
        catch (const std::exception& e)
        {
            GEMINI_ERROR("Failed to save cache registry: {}", e.what());
        }
    }
}
