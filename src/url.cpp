#include "gemini/url.h"

namespace GeminiCPP
{
    ModelName::ModelName(std::string name)
        : value_(std::move(name))
    {
        ensurePrefix();
    }

    ModelName::ModelName(std::string_view name)
        : value_(name)
    {
        ensurePrefix();
    }

    ModelName::ModelName(const char* name)
        : value_(name)
    {
        ensurePrefix();
    }

    ModelName::ModelName(Model model)
        : value_(modelStringRepresentation(model))
    {
        ensurePrefix();
    }

    ModelName& ModelName::operator=(const std::string& name)
    {
        value_ = name;
        ensurePrefix();
        return *this;
    }

    ModelName& ModelName::operator=(std::string_view name)
    {
        value_ = name;
        ensurePrefix();
        return *this;
    }

    ModelName& ModelName::operator=(Model model)
    {
        value_ = modelStringRepresentation(model);
        ensurePrefix();
        return *this;
    }

    std::string ModelName::str() const
    {
        return value_;
    }

    ModelName::operator std::string() const
    {
        return value_;
    }

    void ModelName::ensurePrefix()
    {
        if (!value_.starts_with("models/"))
        {
            value_.insert(0, "models/");
        }
    }

    Url::Url(const ModelName& model, std::string_view action)
    {
        full_url_.reserve(BASE_URL.size() + model.str().size() + action.size());
        full_url_ += BASE_URL;
        full_url_ += model.str();
        full_url_ += action;
    }

    Url::Url(const ModelName& model)
    {
        full_url_.reserve(BASE_URL.size() + model.str().size());
        full_url_ += BASE_URL;
        full_url_ += model.str();
    }

    Url::Url(std::string_view endpoint)
    {
        full_url_.reserve(BASE_URL.size() + endpoint.size());
        full_url_ += BASE_URL;
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
}