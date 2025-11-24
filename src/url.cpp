#include "gemini/url.h"

namespace GeminiCPP
{
    ResourceName::ResourceName(std::string name, ResourceType type)
    : value_(std::move(name)), type_(type)
    {
        ensurePrefix();
    }
    ResourceName::ResourceName(std::string_view name, ResourceType type)
        : value_(name), type_(type)
    {
        ensurePrefix();
    }
    ResourceName::ResourceName(const char* name, ResourceType type)
        : value_(name), type_(type)
    {
        ensurePrefix();
    }
    
    ResourceName::ResourceName(GeminiCPP::Model model)
        : value_(modelStringRepresentation(model)), type_(ResourceType::MODEL)
    {
        ensurePrefix();
    }

    ResourceName ResourceName::File(std::string_view name)
    {
        return {name, ResourceType::FILE};
    }
    ResourceName ResourceName::Model(std::string_view name)
    {
        return {name, ResourceType::MODEL};
    }
    ResourceName ResourceName::TunedModel(std::string_view name)
    {
        return {name, ResourceType::TUNED_MODEL};
    }
    ResourceName ResourceName::Raw(std::string_view name)
    {
        return {name, ResourceType::NONE};
    }

    ResourceName& ResourceName::operator=(const std::string& name)
    {
        value_ = name;
        ensurePrefix();
        return *this;
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
        case ResourceType::MODEL:       prefix = "models/"; break;
        case ResourceType::FILE:        prefix = "files/"; break;
        case ResourceType::TUNED_MODEL: prefix = "tunedModels/"; break;
        case ResourceType::CORPUS:      prefix = "corpora/"; break;
        case ResourceType::OPERATION:   prefix = "operations/"; break;
        case ResourceType::NONE:        return; 
        }

        if (!prefix.empty() && !value_.starts_with(prefix))
        {
            value_.insert(0, prefix);
        }
    }

    Url::Url(const ResourceName& resource, std::string_view action)
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

    Url::Url(std::string_view endpoint, EndpointType type)
    {
        auto base = getBase(type);
        full_url_.reserve(base.size() + endpoint.size());
        full_url_ += base;
        full_url_ += endpoint;
    }

    Url& Url::addQuery(std::string_view key, std::string_view value)
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
