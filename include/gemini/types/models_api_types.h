#pragma once

#ifndef GEMINI_MODELS_API_TYPES_H
#define GEMINI_MODELS_API_TYPES_H

#include <cstdint>
#include <string>

#include "gemini/types_base.h"
#include "gemini/url.h"

namespace GeminiCPP
{
    struct ModelsPredictRequestBody : IJsonSerializable<ModelsPredictRequestBody>
    {
        // Required. The instances that are the input to the prediction call.
        std::vector<nlohmann::json> instances;
        // Optional. The parameters that govern the prediction call.
        std::optional<nlohmann::json> parameters;

        [[nodiscard]] static ModelsPredictRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };
    
    struct ModelsPredictResponseBody : IJsonSerializable<ModelsPredictResponseBody>
    {
        // The outputs of the prediction call.
        std::vector<nlohmann::json> predictions;
        
        [[nodiscard]] static ModelsPredictResponseBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct ModelsPredictLongRunningRequestBody : IJsonSerializable<ModelsPredictLongRunningRequestBody>
    {
        // Required. The instances that are the input to the prediction call.
        std::vector<nlohmann::json> instances;
        // Optional. The parameters that govern the prediction call.
        std::optional<nlohmann::json> parameters;

        [[nodiscard]] static ModelsPredictLongRunningRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    // Information about a Generative Language Model.
    struct ModelInfo : IJsonSerializable<ModelInfo>
    {
        // Required. The resource name of the Model. Refer to Model variants for all allowed values.
        // Format: models/{model} with a {model} naming convention of: "{baseModelId}-{version}"
        // Examples: models/gemini-1.5-flash-001
        ResourceName name;
        // Required. The name of the base model, pass this to the generation request. Examples: gemini-1.5-flash
        ResourceName baseModelId;
        // Required. The version number of the model. This represents the major version (1.0 or 1.5)
        std::string version;
        // The human-readable name of the model. E.g. "Gemini 1.5 Flash". The name can be up to 128 characters long and can consist of any UTF-8 characters.
        std::string displayName;
        // A short description of the model.
        std::string description;
        // Maximum number of input tokens allowed for this model.
        int inputTokenLimit = 0;
        // Maximum number of output tokens available for this model.
        int outputTokenLimit = 0;
        // The model's supported generation methods. The corresponding API method names are defined as Pascal case strings, such as generateMessage and generateContent.
        uint32_t supportedGenerationMethods = GM_NONE;
        // Whether the model supports thinking.
        bool thinking = false;
        // Controls the randomness of the output. Values can range over [0.0,maxTemperature], inclusive. A higher value will produce
        // responses that are more varied, while a value closer to 0.0 will typically result in less surprising responses from the model.
        // This value specifies default to be used by the backend while making the call to the model.
        float temperature = 0.0f;
        // The maximum temperature this model can use.
        float maxTemperature = 0.0f;
        // For Nucleus sampling. Nucleus sampling considers the smallest set of tokens whose probability sum is at least topP.
        // This value specifies default to be used by the backend while making the call to the model.
        float topP = 0.0f;
        // For Top-k sampling. Top-k sampling considers the set of topK most probable tokens. This value
        // specifies default to be used by the backend while making the call to the model. If empty, indicates
        // the model doesn't use top-k sampling, and topK isn't allowed as a generation parameter.
        int topK = 0;

        [[nodiscard]] bool supports(GenerationMethod method) const;
        
        [[nodiscard]] static ModelInfo fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct ModelsGetRequestBody : IJsonSerializable<ModelsGetRequestBody>
    {
        [[nodiscard]] static ModelsGetRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct ModelsListQueryParameters : IJsonSerializable<ModelsListQueryParameters>
    {
        // The maximum number of Models to return (per page).  If unspecified, 50 models will be returned per page.
        // This method returns at most 1000 models per page, even if you pass a larger pageSize.
        int pageSize = 0;
        // A page token, received from a previous models.list call. Provide the pageToken returned by one request as an
        // argument to the next request to retrieve the next page. When paginating, all other parameters provided to
        // models.list must match the call that provided the page token.
        std::string pageToken;

        [[nodiscard]] static ModelsListQueryParameters fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct ModelsListRequestBody : IJsonSerializable<ModelsListRequestBody>
    {
        [[nodiscard]] static ModelsListRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    // Response from ListModel containing a paginated list of Models.
    struct ModelsListResponseBody : IJsonSerializable<ModelsListResponseBody>
    {
        // The returned Models.
        std::vector<ModelInfo> models;
        // A token, which can be sent as pageToken to retrieve the next page. If this field is omitted, there are no more pages.
        std::string nextPageToken;
        
        [[nodiscard]] static ModelsListResponseBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };
}

#endif // GEMINI_MODELS_API_TYPES_H