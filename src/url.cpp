#include "gemini/url.h"

namespace GeminiCPP
{
    ResourceName::ResourceName(std::string name, ResourceType type)
        : value_(std::move(name)), type_(type)
    {
        if (type_ == ResourceType::NONE)
        {
            type_ = detectTypeFromPrefix(value_);
        }
        ensurePrefix();
    }

    ResourceName& ResourceName::operator=(const std::string& name)
    {
        value_ = name;
        type_ = detectTypeFromPrefix(value_);
        return *this;
    }
    
    ResourceName::ResourceName(GeminiCPP::Model model)
        : value_(ModelHelper::stringRepresentation(model)), type_(ResourceType::MODEL)
    {
        ensurePrefix();
    }

    ResourceName ResourceName::File(std::string name)
    {
        return ResourceName{ std::move(name), ResourceType::FILE };
    }
    ResourceName ResourceName::Model(std::string name)
    {
        return ResourceName{ std::move(name), ResourceType::MODEL };
    }

    ResourceName ResourceName::Operation(std::string name)
    {
        return ResourceName{ std::move(name), ResourceType::OPERATION };
    }

    ResourceName ResourceName::CachedContent(std::string name)
    {
        return ResourceName{ std::move(name), ResourceType::CACHED_CONTENT };
    }

    ResourceName ResourceName::Batch(std::string name)
    {
        return ResourceName{ std::move(name), ResourceType::BATCH };
    }

    ResourceName ResourceName::FileSearchStores(std::string name)
    {
        return ResourceName{ std::move(name), ResourceType::FILE_SEARCH_STORES };
    }

    ResourceName ResourceName::Raw(std::string name)
    {
        return ResourceName{ std::move(name), ResourceType::NONE };
    }

    std::string ResourceName::str() const
    {
        return value_;
    }
    ResourceName::operator std::string() const
    {
        return value_;
    }

    void ResourceName::ensurePrefix()
    {
        std::string_view prefix;
        switch (type_)
        {
        case ResourceType::MODEL:           prefix = "models/"; break;
        case ResourceType::FILE:            prefix = "files/"; break;
        case ResourceType::OPERATION:       prefix = "operations/"; break;
        case ResourceType::CACHED_CONTENT:  prefix = "cachedContents/"; break;
        case ResourceType::BATCH:           prefix = "batches/"; break;
        case ResourceType::FILE_SEARCH_STORES:  prefix = "fileSearchStores/"; break;
        case ResourceType::NONE:        return; 
        }

        if (!prefix.empty() && !value_.starts_with(prefix))
        {
            value_.insert(0, prefix);
        }
    }

    ResourceType ResourceName::detectTypeFromPrefix(std::string_view s)
    {
        if (s.starts_with("models/"))           return ResourceType::MODEL;
        if (s.starts_with("files/"))            return ResourceType::FILE;
        if (s.starts_with("operations/"))       return ResourceType::OPERATION;
        if (s.starts_with("cachedContents/"))   return ResourceType::CACHED_CONTENT;
        if (s.starts_with("batches/"))          return ResourceType::BATCH;
        if (s.starts_with("fileSearchStores/")) return ResourceType::FILE_SEARCH_STORES;
        return ResourceType::NONE;
    }

    Url::Url(const ResourceName& resource, const std::string& action)
    {
        auto base = getBase(EndpointType::REST);
        full_url_.reserve(base.size() + resource.str().size() + action.size());
        full_url_ += base;
        full_url_ += resource.str();
        full_url_ += action;
    }

    Url::Url(const ResourceName& resource, GenerationMethod action)
    {
        auto base = getBase(EndpointType::REST);
        std::string actionStr = GenerationMethodHelper::toString(action);
        
        full_url_.reserve(base.size() + resource.str().size() + 1 + actionStr.size());
        
        full_url_ += base;
        full_url_ += resource.str();
        full_url_ += ':';
        full_url_ += actionStr;
    }

    Url::Url(const ResourceName& resource)
    {
        auto base = getBase(EndpointType::REST);
        full_url_.reserve(base.size() + resource.str().size());
        full_url_ += base;
        full_url_ += resource.str();
    }

    Url::Url(const std::string& endpoint, EndpointType type)
    {
        if (endpoint.starts_with("http://") || endpoint.starts_with("https://"))
        {
            full_url_ = endpoint;
        }
        else
        {
            auto base = getBase(type);
            full_url_.reserve(base.size() + endpoint.size());
            full_url_ += base;
            full_url_ += endpoint;
        }
    }

    Url& Url::addQuery(const std::string& key, const std::string& value)
    {
        full_url_ += (has_query_ ? "&" : "?");
        full_url_ += key;
        if (!value.empty())
        {
            full_url_ += "=";
            full_url_ += value;
        }
        has_query_ = true;
        return *this;
    }

    std::string Url::str() const
    {
        return full_url_;
    }

    Url::operator std::string() const
    {
        return full_url_;
    }

    std::string_view Url::getBase(EndpointType type)
    {
        switch (type)
        {
        case EndpointType::REST: return BASE_URL_REST;
        case EndpointType::UPLOAD: return BASE_URL_UPLOAD;
        }
        return BASE_URL_REST;
    }
}
