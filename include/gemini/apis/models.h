#pragma once

#ifndef GEMINI_APIS_MODELS_H
#define GEMINI_APIS_MODELS_H

#include <string>
#include "gemini/response.h"
#include "gemini/types/models_api_types.h"

namespace GeminiCPP
{
    class Client;

    /**
     * @brief Provides methods to query available models.
     * * Use this module to discover model capabilities, version information,
     * and input/output token limits.
     */
    class Models
    {
    public:
        /**
         * @brief Constructs a new Models API module.
         * @param client Pointer to the main Client instance.
         */
        explicit Models(Client* client);

        /**
         * @brief Gets information about a specific model.
         * @param name The resource name of the model (e.g., "models/gemini-1.5-flash").
         * @return Result<ModelInfo> containing model details.
         */
        Result<ModelInfo> get(const std::string& name);

        /**
         * @brief Lists available models.
         * @param pageSize Maximum number of models to return (default 50).
         * @param pageToken Token for the next page of results.
         * @return Result<ModelsListResponseBody> containing the list of models.
         */
        Result<ModelsListResponseBody> list(int pageSize = 50, const std::string& pageToken = "");

    private:
        Client* client_;
    };
}

#endif // GEMINI_APIS_MODELS_H