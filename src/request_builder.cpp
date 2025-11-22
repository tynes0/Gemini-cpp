#include "gemini/request_builder.h"
#include "gemini/client.h"

namespace GeminiCPP
{
    RequestBuilder::RequestBuilder(Client* client) : client_(client) {}

    RequestBuilder& RequestBuilder::model(Model m)
    {
        model_ = m;
        return *this;
    }

    RequestBuilder& RequestBuilder::systemInstruction(const std::string& instruction)
    {
        systemInstruction_ = instruction;
        return *this;
    }

    RequestBuilder& RequestBuilder::text(const std::string& t)
    {
        parts_.push_back(Part::Text(t));
        return *this;
    }

    RequestBuilder& RequestBuilder::image(const std::string& filepath)
    {
        parts_.push_back(Part::Media(filepath));
        return *this;
    }

    GenerationResult RequestBuilder::generate() const
    {
        return client_->generateFromBuilder(model_, systemInstruction_, parts_);
    }

}