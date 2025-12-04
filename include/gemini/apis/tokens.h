#pragma once

#ifndef GEMINI_APIS_TOKENS_H
#define GEMINI_APIS_TOKENS_H

#include <string>
#include "gemini/response.h"
#include "gemini/types/tokens_api_types.h"

namespace GeminiCPP
{
    class Client;

    /**
     * @brief Provides methods for counting tokens.
     * * Useful for estimating costs and ensuring requests stay within model context limits.
     */
    class Tokens
    {
    public:
        /**
         * @brief Constructs a new Tokens API module.
         * @param client Pointer to the main Client instance.
         */
        explicit Tokens(Client* client);

        /**
         * @brief Counts tokens for a complex request (multi-modal, tools, etc.).
         * @param model The model to use for tokenization (e.g., "models/gemini-1.5-flash").
         * @param request The full token counting request body containing the content.
         * @return Result<CountTokensResponseBody> containing the total token count.
         */
        Result<CountTokensResponseBody> count(const std::string& model, const CountTokensRequestBody& request);

        /**
         * @brief Convenience overload to count tokens for a simple text string.
         * @param model The model to use for tokenization.
         * @param text The text content to tokenize.
         * @return Result<CountTokensResponseBody> containing the total token count.
         */
        Result<CountTokensResponseBody> count(const std::string& model, const std::string& text);

    private:
        Client* client_;
    };
}

#endif // GEMINI_APIS_TOKENS_H