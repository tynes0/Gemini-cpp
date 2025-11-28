#include "gemini/types/embeddings_api_types.h"

namespace GeminiCPP
{
    nlohmann::json EmbedRequestBody::toJson() const
    {
        nlohmann::json j;
        
        if (taskType.has_value()) 
        {
            j["taskType"] = frenum::to_string(taskType.value());
        }
        
        if (!title.empty())
            j["title"] = title;
        
        if (outputDimensionality.has_value())
            j["outputDimensionality"] = outputDimensionality.value();
        
        return j;
    }

    ContentEmbedding ContentEmbedding::fromJson(const nlohmann::json& j)
    {
        ContentEmbedding ce;
        if(j.contains("values") && j["values"].is_array())
        {
            for(const auto& v : j["values"])
                ce.values.push_back(v.get<float>());
        }
        return ce;
    }

    EmbedContentResponse EmbedContentResponse::fromJson(const nlohmann::json& j)
    {
        EmbedContentResponse r;
        if(j.contains("embedding"))
            r.embedding = ContentEmbedding::fromJson(j["embedding"]);
        return r;
    }

    BatchEmbedContentsResponse BatchEmbedContentsResponse::fromJson(const nlohmann::json& j)
    {
        BatchEmbedContentsResponse r;
        if(j.contains("embeddings") && j["embeddings"].is_array())
        {
            for(const auto& item : j["embeddings"])
            {
                r.embeddings.push_back(ContentEmbedding::fromJson(item));
            }
        }
        return r;
    }
}
