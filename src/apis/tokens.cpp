#include "gemini/apis/tokens.h"
#include "gemini/client.h"
#include "gemini/url.h"

namespace GeminiCPP
{
    Tokens::Tokens(Client* client)
    : client_(client)
    {
    }

    Result<CountTokensResponseBody> Tokens::count(const std::string& model, const CountTokensRequestBody& request)
    {
        ResourceName modelResource(model, ResourceType::MODEL);
        Url url(modelResource, GM_COUNT_TOKENS);
        
        return client_->post<CountTokensResponseBody>(url.str(), request.toJson());
    }

    Result<CountTokensResponseBody> Tokens::count(const std::string& model, const std::string& text)
    {
        CountTokensRequestBody req;
        
        req.contents = std::vector<Content>{ Content::User().text(text) };
        
        return count(model, req);
    }
}