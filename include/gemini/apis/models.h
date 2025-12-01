#pragma once

#ifndef GEMINI_APIS_MODELS_H
#define GEMINI_APIS_MODELS_H

#include <string>
#include "gemini/response.h"
#include "gemini/types/models_api_types.h"

namespace GeminiCPP
{
    class Client;

    class Models
    {
    public:
        explicit Models(Client* client);

        /**
         * @brief Gets information about a specific model.
         * @param name The resource name of the model (e.g., "models/gemini-pro").
         */
        Result<ModelInfo> get(const std::string& name);

        /**
         * @brief Lists available models.
         * @param pageSize Maximum number of models to return.
         * @param pageToken Token for the next page.
         */
        Result<ModelsListResponseBody> list(int pageSize = 50, const std::string& pageToken = "");

    private:
        Client* client_;
    };
}

#endif // GEMINI_APIS_MODELS_H