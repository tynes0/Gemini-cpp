#pragma once

#ifndef GEMINI_APIS_TOKENS_H
#define GEMINI_APIS_TOKENS_H

#include <string>
#include "gemini/response.h"
#include "gemini/types/tokens_api_types.h"

namespace GeminiCPP
{
    class Client;

    class Tokens
    {
    public:
        explicit Tokens(Client* client);

        /**
         * @brief Counts tokens for a given request.
         * @param model The model to use for counting.
         * @param request The full request body.
         */
        Result<CountTokensResponseBody> count(const std::string& model, const CountTokensRequestBody& request);

        /**
         * @brief Helper overload to count tokens for a simple text string.
         */
        Result<CountTokensResponseBody> count(const std::string& model, const std::string& text);

    private:
        Client* client_;
    };
}

#endif // GEMINI_APIS_TOKENS_H