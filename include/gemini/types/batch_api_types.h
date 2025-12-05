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
    /**
     * @brief State of a Batch Job.
     */
    FrenumClassInNamespace(GeminiCPP, BatchState, uint8_t,
        BATCH_STATE_UNSPECIFIED,
        BATCH_STATE_PENDING, // The service is preparing to run the batch.
        BATCH_STATE_RUNNING, // The batch is in progress.
        BATCH_STATE_SUCCEEDED, // The batch completed successfully.
        BATCH_STATE_FAILED, // The batch failed.
        BATCH_STATE_CANCELLED, // The batch has been cancelled.
        BATCH_STATE_EXPIRED // The batch has expired.
    )
    
    /**
     * @brief Represents a Long-Running Operation (LRO).
     * * Used for operations that take significant time, like batch processing or model tuning.
     */
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

    struct ListOperationsRequest : IJsonSerializable<ListOperationsRequest>
    {
        // The name of the operation's parent resource.
        std::string name;
        // The standard list filter.
        std::string filter;
        // The standard list page size.
        int pageSize = 0;
        // The standard list page token.
        std::string pageToken;
        // When set to true, operations that are reachable are returned as normal, and those that are unreachable are
        // returned in the ListOperationsResponse.unreachable field. This can only be true when reading across collections.
        // For example, when parent is set to 'projects/example/locations/-'. This field is not supported by default and will
        // result in an UNIMPLEMENTED error if set unless explicitly documented otherwise in service or product specific documentation.
        bool returnPartialSuccess = false;
        
        [[nodiscard]] static ListOperationsRequest fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    // The response message for Operations.ListOperations.
    struct ListOperationsResponse : IJsonSerializable<ListOperationsResponse>
    {
        // A list of operations that matches the specified filter in the request.
        std::vector<Operation> operations;
        // The standard List next-page token.
        std::string nextPageToken;
        // Unordered list. Unreachable resources. Populated when the request sets ListOperationsRequest.return_partial_success
        // and reads across collections. For example, when attempting to list all resources across all supported locations.
        std::vector<std::string> unreachable;

        [[nodiscard]] static ListOperationsResponse fromJson(const nlohmann::json& j);
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

    // The request to be processed in the batch.
    struct InlinedRequest : IJsonSerializable<InlinedRequest>
    {
        // Required. The request to be processed in the batch.
        GenerateContentRequestBody request;
        // Optional. The metadata to be associated with the request.
        std::optional<nlohmann::json> metadata;

        [[nodiscard]] static InlinedRequest fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    // The requests to be processed in the batch if provided as part of the batch creation request.
    struct InlinedRequests : IJsonSerializable<InlinedRequests>
    {
        // Required. The requests to be processed in the batch.
        std::vector<InlinedRequest> requests;
        
        [[nodiscard]] static InlinedRequests fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    // Configures the input to the batch request.
    struct InputConfig : IJsonSerializable<InputConfig>
    {
        using SourceType = std::variant<
            std::monostate,
            // The name of the File containing the input requests.
            std::string,
            // The requests to be processed in the batch.
            InlinedRequests
        >;

        // Required. The source of the input.
        SourceType source;
        
        [[nodiscard]] static InputConfig fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    // A resource representing a batch of GenerateContent requests.
    struct GenerateContentBatch : IJsonSerializable<GenerateContentBatch>
    {
        // Required. The name of the Model to use for generating the completion. Format: models/{model}.
        ResourceName model;
        // Output only. Identifier. Resource name of the batch. Format: batches/{batchId}.
        ResourceName name;
        // Required. The user-defined name of this batch.
        std::string displayName;
        // Required. Input configuration of the instances on which batch processing are performed.
        InputConfig inputConfig;
        // Output only. The output of the batch request.
        GenerateContentBatchOutput output;
        // Output only. The time at which the batch was created.
        Support::Timestamp createTime;
        // Output only. The time at which the batch processing completed.
        Support::Timestamp endTime;
        // Output only. The time at which the batch was last updated.
        Support::Timestamp updateTime;
        // Output only. Stats about the batch.
        BatchStats batchStats;
        // Output only. The state of the batch.
        BatchState state;
        // Optional. The priority of the batch. Batches with a higher priority value will be processed before
        // batches with a lower priority value. Negative values are allowed. Default is 0.
        std::optional<int64_t> priority; 
        
        [[nodiscard]] static GenerateContentBatch fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct ModelsBatchGenerateContentRequestBody : IJsonSerializable<ModelsBatchGenerateContentRequestBody>
    {
        GenerateContentBatch request;

        [[nodiscard]] static ModelsBatchGenerateContentRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct ModelsBatchGenerateContentResponseBody : IJsonSerializable<ModelsBatchGenerateContentResponseBody>
    {
        Operation response;

        [[nodiscard]] static ModelsBatchGenerateContentResponseBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct ModelsAsyncBatchGenerateContentRequestBody : IJsonSerializable<ModelsAsyncBatchGenerateContentRequestBody>
    {
        GenerateContentBatch request;

        [[nodiscard]] static ModelsAsyncBatchGenerateContentRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct ModelsAsyncBatchGenerateContentResponseBody : IJsonSerializable<ModelsAsyncBatchGenerateContentResponseBody>
    {
        Operation response;

        [[nodiscard]] static ModelsAsyncBatchGenerateContentResponseBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct BatchesGetRequestBody : IJsonSerializable<BatchesGetRequestBody>
    {
        [[nodiscard]] static BatchesGetRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct BatchesGetResponseBody : IJsonSerializable<BatchesGetResponseBody>
    {
        Operation response;
        
        [[nodiscard]] static BatchesGetResponseBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };
    
    struct BatchesListRequestBody : IJsonSerializable<BatchesListRequestBody>
    {
        [[nodiscard]] static BatchesListRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct BatchesListResponseBody : IJsonSerializable<BatchesListResponseBody>
    {
        ListOperationsResponse response;
        
        [[nodiscard]] static BatchesListResponseBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct BatchesCancelRequestBody : IJsonSerializable<BatchesCancelRequestBody>
    {
        [[nodiscard]] static BatchesCancelRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct BatchesCancelResponseBody : IJsonSerializable<BatchesCancelResponseBody>
    {
        [[nodiscard]] static BatchesCancelResponseBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct BatchesDeleteRequestBody : IJsonSerializable<BatchesDeleteRequestBody>
    {
        [[nodiscard]] static BatchesDeleteRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct BatchesDeleteResponseBody : IJsonSerializable<BatchesDeleteResponseBody>
    {
        [[nodiscard]] static BatchesDeleteResponseBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct BatchesUpdateEmbedContentBatchQueryParameters : IJsonSerializable<BatchesUpdateEmbedContentBatchQueryParameters>
    {
        // Optional. The list of fields to update. This is a comma-separated list of fully qualified names of fields. Example: "user.displayName,photo".
        Support::FieldMask updateMask;
        
        [[nodiscard]] static BatchesUpdateEmbedContentBatchQueryParameters fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };

    struct BatchesUpdateEmbedContentBatchRequestBody : IJsonSerializable<BatchesUpdateEmbedContentBatchRequestBody>
    {
        // TODO:
        
        [[nodiscard]] static BatchesUpdateEmbedContentBatchRequestBody fromJson(const nlohmann::json& j);
        [[nodiscard]] nlohmann::json toJson() const override;
    };
}

#endif // GEMINI_BATCH_API_TYPES_H