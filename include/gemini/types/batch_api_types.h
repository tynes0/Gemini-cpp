#pragma once

#ifndef GEMINI_BATCH_API_TYPES_H
#define GEMINI_BATCH_API_TYPES_H

#include <string>
#include <optional>
#include <vector>

#include "caching_api_types.h"
#include "embeddings_api_types.h"
#include "gemini/url.h"
#include "gemini/types_base.h"
#include "files_api_types.h"
#include "generating_content_api_types.h"

namespace GeminiCPP
{
    // The state of the batch.
FrenumClassInNamespace(GeminiCPP, BatchState, uint8_t,
                           BATCH_STATE_UNSPECIFIED,
                           BATCH_STATE_PENDING, // The service is preparing to run the batch.
                           BATCH_STATE_RUNNING, // The batch is in progress.
                           BATCH_STATE_SUCCEEDED, // The batch completed successfully.
                           BATCH_STATE_FAILED, // The batch failed.
                           BATCH_STATE_CANCELLED, // The batch has been cancelled.
                           BATCH_STATE_EXPIRED // The batch has expired.
    )
    // This resource represents a long-running operation that is the result of a network API call.
    struct Operation : IJsonSerializable<Operation>
    {
        using ResultType = std::variant<
            std::monostate,
            // The error result of the operation in case of failure or cancellation.
            Status,
            // The normal, successful response of the operation. If the original method returns no data on success, such as Delete,
            // the response is google.protobuf.Empty. If the original method is standard Get/Create/Update, the response should be the resource.
            // For other methods, the response should have the type XxxResponse, where Xxx is the original method name. For example,
            // if the original method name is TakeSnapshot(), the inferred response type is TakeSnapshotResponse. An object containing
            // fields of an arbitrary type. An additional field "@type" contains a URI identifying the type.
            // Example: { "id": 1234, "@type": "types.example.com/standard/id" }.
            nlohmann::json
        >;

        // The server-assigned name, which is only unique within the same service that originally returns it.
        // If you use the default HTTP mapping, the name should be a resource name ending with operations/{unique_id}.
        ResourceName name;
        // Service-specific metadata associated with the operation. It typically contains progress information and common
        // metadata such as create time. Some services might not provide such metadata. Any method that returns a
        // long-running operation should document the metadata type, if any. An object containing fields of an arbitrary type.
        // An additional field "@type" contains a URI identifying the type. Example: { "id": 1234, "@type": "types.example.com/standard/id" }.
        nlohmann::json metadata;
        // If the value is false, it means the operation is still in progress. If true, the operation is completed, and either error or response is available.
        bool done = false;
        // The operation result, which can be either an error or a valid response. If done == false, neither error nor response is set.
        // If done == true, exactly one of error or response can be set. Some services might not provide the result.
        ResultType result;

        [[nodiscard]] static Operation fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
        
    };

    // Request containing the Content for the model to embed.
    struct EmbedContentRequest : IJsonSerializable<EmbedContentRequest>
    {
        // Required. The model's resource name. This serves as an ID for the Model to use.
        // This name should match a model name returned by the ListModels method. Format: models/{model}
        ResourceName name;
        // Required. The content to embed. Only the parts.text fields will be counted.
        Content content;
        // Optional. Optional task type for which the embeddings will be used. Not supported on earlier models (models/embedding-001).
        std::optional<TaskType> taskType;
        // Optional. An optional title for the text. Only applicable when TaskType is RETRIEVAL_DOCUMENT.
        // Note: Specifying a title for RETRIEVAL_DOCUMENT provides better quality embeddings for retrieval.
        std::optional<std::string> title;
        // Optional. Optional reduced dimension for the output embedding. If set, excessive values in
        // the output embedding are truncated from the end. Supported by newer models since 2024 only.
        // You cannot set this value if using the earlier model (models/embedding-001).
        std::optional<int> outputDimensionality;
        
        [[nodiscard]] static EmbedContentRequest fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    // Stats about the batch.
    struct BatchStats : IJsonSerializable<BatchStats>
    {
        // Output only. The number of requests in the batch.
        int64_t requestCount = 0;
        // Output only. The number of requests that were successfully processed.
        int64_t successfulRequestCount = 0;
        // Output only. The number of requests that failed to be processed.
        int64_t failedRequestCount = 0;
        // Output only. The number of requests that are still pending processing.
        int64_t pendingRequestCount = 0;

        [[nodiscard]] static BatchStats fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct InlinedResponse : IJsonSerializable<InlinedResponse>
    {
        using OutputType = std::variant<
            std::monostate,
            // Output only. The error encountered while processing the request.
            Status,
            // Output only. The response to the request.
            GenerateContentResponseBody
        >;
        
        // Output only. The metadata associated with the request.
        nlohmann::json metadata;
        // The output of the request. 
        OutputType output;
        
        [[nodiscard]] static InlinedResponse fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    // The responses to the requests in the batch.
    struct InlinedResponses : IJsonSerializable<InlinedResponses>
    {
        // Output only. The responses to the requests in the batch.
        std::vector<InlinedResponse> inlinedResponses;
        
        [[nodiscard]] static InlinedResponses fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    // The output of a batch request. This is returned in the BatchGenerateContentResponse or the GenerateContentBatch.output field.
    struct GenerateContentBatchOutput : IJsonSerializable<GenerateContentBatchOutput>
    {
        using OutputType = std::variant<
            std::monostate,
            // Output only. The file ID of the file containing the responses. The file will be a JSONL file with a single response per line.
            // The responses will be GenerateContentResponse messages formatted as JSON. The responses will be written in the same order as the input requests.
            std::string,
            // Output only. The responses to the requests in the batch. Returned when the batch was
            // built using inlined requests. The responses will be in the same order as the input requests.
            InlinedResponses
        >;

        // The output of the batch request. 
        OutputType output;
        
        [[nodiscard]] static GenerateContentBatchOutput fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };
}

#endif // GEMINI_BATCH_API_TYPES_H