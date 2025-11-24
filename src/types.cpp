#include "gemini/types.h"

#include "gemini/utils.h"

namespace GeminiCPP
{
    nlohmann::json FunctionDeclaration::toJson() const
    {
        return {
            {"name", name},
            {"description", description},
            {"parameters", parameters}
        };
    }

    nlohmann::json GoogleSearch::toJson() const
    {
        return nlohmann::json::object();
    }

    nlohmann::json Tool::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
            
        if (!functionDeclarations.empty())
        {
            nlohmann::json funcs = nlohmann::json::array();
            for(const auto& f : functionDeclarations)
                funcs.push_back(f.toJson());
            
            j["function_declarations"] = funcs;
        }
            
        if (googleSearch.has_value())
        {
            j["googleSearch"] = googleSearch->toJson();
        }
            
        return j;
    }

    nlohmann::json FunctionCall::toJson() const
    {
        return {
            {"name", name},
            {"args", args}
        };
    }

    nlohmann::json FunctionResponse::toJson() const
    {
        return {
                {"name", name},
                {"response", {
                {"name", name},
                {"content", response}
                }}
        };
    }

    bool Part::isText() const
    {
        return std::holds_alternative<TextData>(content);
    }
    bool Part::isBlob() const
    {
        return std::holds_alternative<BlobData>(content);
    }

    bool Part::isFileData() const
    {
        return std::holds_alternative<FileData>(content);
    }
    
    bool Part::isFunctionCall() const
    {
        return std::holds_alternative<FunctionCall>(content);
    }
    bool Part::isFunctionResponse() const
    {
        return std::holds_alternative<FunctionResponse>(content);
    }

    const std::string* Part::getText() const
    {
        if(auto* p = std::get_if<TextData>(&content))
            return &p->text;
        return nullptr;
    }

    const BlobData* Part::getBlob() const
    {
        return std::get_if<BlobData>(&content);
    }

    const FileData* Part::getFileData() const
    {
        return std::get_if<FileData>(&content);
    }

    const FunctionCall* Part::getFunctionCall() const
    {
        return std::get_if<FunctionCall>(&content);
    }

    const FunctionResponse* Part::getFunctionResponse() const
    {
        return std::get_if<FunctionResponse>(&content);
    }

    Part Part::Text(std::string t)
    {
        Part p;
        p.content = TextData{
            std::move(t)
        };
        return p;
    }

    Part Part::Media(const std::string& filepath, const std::string& customMimeType)
    {
        Part p;
        p.content = BlobData{
            customMimeType.empty() ? Utils::getMimeType(filepath) : customMimeType,
            Utils::fileToBase64(filepath)
        };
        return p;
    }

    Part Part::Uri(std::string fileUri, std::string mimeType)
    {
        Part p;
        p.content = FileData{
            std::move(mimeType),
            std::move(fileUri)
        };
        return p;
    }

    Part Part::Call(FunctionCall call)
    {
        Part p;
        p.content = std::move(call);
        return p;
    }

    Part Part::Response(FunctionResponse resp)
    {
        Part p;
        p.content = std::move(resp);
        return p;
    }

    nlohmann::json Part::toJson() const
    {
        return std::visit([](const auto& arg) -> nlohmann::json {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::monostate>)
            {
                return nlohmann::json::object();
            }
            else if constexpr (std::is_same_v<T, TextData>)
            {
                return { {"text", arg.text} };
            }
            else if constexpr (std::is_same_v<T, BlobData>)
            {
                return {
                    {"inlineData", {
                        {"mimeType", arg.mimeType},
                        {"data", arg.data}
                    }}
                };
            }
            else if constexpr (std::is_same_v<T, FileData>) {
                return { 
                    {"fileData", { 
                        {"mimeType", arg.mimeType}, 
                        {"fileUri", arg.fileUri} 
                    }} 
                };
            }
            else if constexpr (std::is_same_v<T, FunctionCall>)
            {
                return { {"functionCall", arg.toJson()} };
            }
            else if constexpr (std::is_same_v<T, FunctionResponse>)
            {
                return { {"functionResponse", arg.toJson()} };
            }

            return {"", {}};
        }, content);
    }

    Content Content::User()
    {
        return {Role::USER, {}};
    }

    Content Content::Model()
    {
        return {Role::MODEL, {}};
    }

    Content Content::Function()
    {
        return {Role::FUNCTION, {}};
    }

    Content& Content::text(const std::string& t)
    {
        parts.push_back(Part::Text(t));
        return *this;
    }

    Content& Content::image(const std::string& filepath)
    {
        return media(filepath);
    }

    Content& Content::file(const std::string& filepath)
    {
        return media(filepath);
    }

    Content& Content::fileUri(const std::string& uri, const std::string& mimeType)
    {
        parts.push_back(Part::Uri(uri, mimeType));
        return *this;
    }

    Content& Content::media(const std::string& filepath, const std::string& mimeType)
    {
        parts.push_back(Part::Media(filepath, mimeType));
        return *this;
    }

    Content& Content::functionResponse(std::string name, nlohmann::json responseContent)
    {
        parts.push_back(Part::Response({std::move(name), std::move(responseContent)}));
        role = Role::FUNCTION; 
        return *this;
    }

    nlohmann::json Content::toJson() const
    {
        nlohmann::json partsJson = nlohmann::json::array();
        
        for (const auto& p : parts)
            partsJson.push_back(p.toJson());
            
        std::string roleStr = "user";
        if (role == Role::MODEL)
            roleStr = "model";
        if (role == Role::FUNCTION)
            roleStr = "function";
            
        return { {"role", roleStr}, {"parts", partsJson} };
    }

    Content Content::fromJson(const nlohmann::json& j)
    {
        Content c;
        c.role = Role::MODEL;
            
        if (j.contains("role"))
        {
            std::string r = j["role"];
            if (r == "user") c.role = Role::USER;
            else if (r == "model") c.role = Role::MODEL;
            else if (r == "function") c.role = Role::FUNCTION;
        }
    
        if (j.contains("parts"))
        {
            for (const auto& item : j["parts"])
            {
                if (item.contains("text")) {
                    c.parts.push_back(Part::Text(item["text"].get<std::string>()));
                }
                else if (item.contains("inlineData")) {
                    Part p;
                    p.content = BlobData{
                        item["inlineData"]["mimeType"].get<std::string>(),
                        item["inlineData"]["data"].get<std::string>()
                    };
                    c.parts.push_back(p);
                }
                else if (item.contains("fileData")) {
                    Part p;
                    p.content = FileData{
                        item["fileData"].value("mimeType", ""),
                        item["fileData"].value("fileUri", "")
                    };
                    c.parts.push_back(p);
                }
                else if (item.contains("functionCall")) {
                    auto fc = item["functionCall"];
                    c.parts.push_back(Part::Call({
                        fc["name"].get<std::string>(),
                        fc["args"]
                    }));
                }
            }
        }
        return c;
    }

    nlohmann::json GenerationConfig::toJson() const
    {
        nlohmann::json j;
            
        if (responseMimeType.has_value()) j["responseMimeType"] = responseMimeType.value();
        if (stopSequences.has_value())    j["stopSequences"] = stopSequences.value();
        if (temperature.has_value())      j["temperature"] = temperature.value();
        if (topP.has_value())             j["topP"] = topP.value();
        if (topK.has_value())             j["topK"] = topK.value();
        if (candidateCount.has_value())   j["candidateCount"] = candidateCount.value();
        if (maxOutputTokens.has_value())  j["maxOutputTokens"] = maxOutputTokens.value();
        if (responseLogprobs.has_value()) j["responseLogprobs"] = responseLogprobs.value();
        if (logprobs.has_value())         j["logprobs"] = logprobs.value();
        if (presencePenalty.has_value())  j["presencePenalty"] = presencePenalty.value();
        if (frequencyPenalty.has_value()) j["frequencyPenalty"] = frequencyPenalty.value();
        if (seed.has_value())             j["seed"] = seed.value();
        if (responseSchema.has_value())   j["responseSchema"] = responseSchema.value();

        return j;
    }

    nlohmann::json EmbedConfig::toJson() const
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

    std::string SafetySetting::categoryToString(HarmCategory cat)
    {
        return frenum::to_string(cat);
    }

    std::string SafetySetting::thresholdToString(HarmBlockThreshold thr)
    {
        return frenum::to_string(thr);
    }

    nlohmann::json SafetySetting::toJson() const
    {
        return {
            {"category", categoryToString(category)},
            {"threshold", thresholdToString(threshold)}
        };
    }

    File File::fromJson(const nlohmann::json& j)
    {
        File f{};
        
        if(j.contains("name")) f.name = j.value("name", "");
        if(j.contains("displayName")) f.displayName = j.value("displayName", "");
        if(j.contains("mimeType")) f.mimeType = j.value("mimeType", "");
        if(j.contains("sizeBytes")) f.sizeBytes = j.value("sizeBytes", "0");
        if(j.contains("createTime")) f.createTime = j.value("createTime", "");
        if(j.contains("updateTime")) f.updateTime = j.value("updateTime", "");
        if(j.contains("expirationTime")) f.expirationTime = j.value("expirationTime", "");
        if(j.contains("sha256Hash")) f.sha256Hash = j.value("sha256Hash", "");
        if(j.contains("uri")) f.uri = j.value("uri", "");
            
        if(j.contains("state"))
        {
            auto s = frenum::cast<FileState>(j.value("state", ""));
            if(s.has_value()) f.state = s.value();
        }
        
        if(j.contains("error") && j["error"].contains("message"))
        {
            f.errorMsg = j["error"]["message"].get<std::string>();
        }

        return f;
    }

    std::string File::toString() const
    {
        return "File: " + displayName + " (" + name + ")\n" +
               "URI: " + uri + "\n" +
               "State: " + frenum::to_string(state) + "\n" +
               "MIME: " + mimeType + " (" + sizeBytes + " bytes)";
    }

    ListFilesResponse ListFilesResponse::fromJson(const nlohmann::json& j)
    
    {
        ListFilesResponse r;
        if(j.contains("files") && j["files"].is_array())
        {
            for(const auto& item : j["files"])
            {
                r.files.push_back(File::fromJson(item));
            }
        }
        if(j.contains("nextPageToken")) r.nextPageToken = j.value("nextPageToken", "");
        return r;
    }

    FinishReason FinishReasonHelper::fromString(const std::string& reason)
    {
        auto result = frenum::cast<FinishReason>(reason);
        return result.has_value() ? result.value() : FinishReason::FINISH_REASON_UNSPECIFIED;
    }

    std::string FinishReasonHelper::toString(FinishReason reason)
    {
        return frenum::to_string(reason);
    }

    GenerationMethod GenerationMethodHelper::fromString(const std::string& method)
    {
        if (method == "asyncBatchEmbedContent") return GM_ASYNC_BATCH_EMBED_CONTENT;
        if (method == "batchEmbedContents")     return GM_BATCH_EMBED_CONTENTS;
        if (method == "batchEmbedText")         return GM_BATCH_EMBED_TEXT;
        if (method == "batchGenerateContent")   return GM_BATCH_GENERATE_CONTENT;
        if (method == "countMessageTokens")     return GM_COUNT_MESSAGE_TOKENS;
        if (method == "countTextTokens")        return GM_COUNT_TEXT_TOKENS;
        if (method == "countTokens")            return GM_COUNT_TOKENS;
        if (method == "embedContent")           return GM_EMBED_CONTENT;
        if (method == "embedText")              return GM_EMBED_TEXT;
        if (method == "generateContent")        return GM_GENERATE_CONTENT;
        if (method == "generateMessage")        return GM_GENERATE_MESSAGE;
        if (method == "generateText")           return GM_GENERATE_TEXT;
        if (method == "get")                    return GM_GET;
        if (method == "list")                   return GM_LIST;
        if (method == "predict")                return GM_PREDICT;
        if (method == "predictLongRunning")     return GM_PREDICT_LONG_RUNNING;
        if (method == "streamGenerateContent")  return GM_STREAM_GENERATE_CONTENT;
        return GM_UNSPECIFIED;
    }

    std::string GenerationMethodHelper::toString(GenerationMethod method)
    {
        switch (method)
        {
        case GM_ASYNC_BATCH_EMBED_CONTENT:  return "asyncBatchEmbedContents";
        case GM_BATCH_EMBED_CONTENTS:       return "batchEmbedContents";
        case GM_BATCH_EMBED_TEXT:           return "batchEmbedText";
        case GM_BATCH_GENERATE_CONTENT:     return "batchGenerateContent";
        case GM_COUNT_MESSAGE_TOKENS:       return "countMessageTokens";
        case GM_COUNT_TEXT_TOKENS:          return "countTextTokens";
        case GM_COUNT_TOKENS:               return "countTokens";
        case GM_EMBED_CONTENT:              return "embedContent";
        case GM_EMBED_TEXT:                 return "embedText";
        case GM_GENERATE_CONTENT:           return "generateContent";
        case GM_GENERATE_MESSAGE:           return "generateMessage";
        case GM_GENERATE_TEXT:              return "generateText";
        case GM_GET:                        return "get";
        case GM_LIST:                       return "list";
        case GM_PREDICT:                    return "predict";
        case GM_PREDICT_LONG_RUNNING:       return "predictLongRunning";
        case GM_STREAM_GENERATE_CONTENT:    return "streamGenerateContent";
        case GM_NONE:
        case GM_UNSPECIFIED:
            break;
        }
        return "";    
    }

    std::string GenerationMethodHelper::bitmaskToString(uint32_t flags)
    {
        std::string s;
        if (flags & GM_ASYNC_BATCH_EMBED_CONTENT)   s += "asyncBatchEmbedContents, ";
        if (flags & GM_BATCH_EMBED_CONTENTS)        s += "batchEmbedContents, ";
        if (flags & GM_BATCH_EMBED_TEXT)            s += "batchEmbedText, ";
        if (flags & GM_BATCH_GENERATE_CONTENT)      s += "batchGenerateContent, ";
        if (flags & GM_COUNT_MESSAGE_TOKENS)        s += "countMessageTokens, ";
        if (flags & GM_COUNT_TEXT_TOKENS)           s += "countTextTokens, ";
        if (flags & GM_COUNT_TOKENS)                s += "countTokens, ";
        if (flags & GM_EMBED_CONTENT)               s += "embedContent, ";
        if (flags & GM_EMBED_TEXT)                  s += "embedText, ";
        if (flags & GM_GENERATE_CONTENT)            s += "generateContent, ";
        if (flags & GM_GENERATE_MESSAGE)            s += "generateMessage, ";
        if (flags & GM_GENERATE_TEXT)               s += "generateText, ";
        if (flags & GM_GET)                         s += "get, ";
        if (flags & GM_LIST)                        s += "list, ";
        if (flags & GM_PREDICT)                     s += "predict, ";
        if (flags & GM_PREDICT_LONG_RUNNING)        s += "predictLongRunning, ";
        if (flags & GM_STREAM_GENERATE_CONTENT)     s += "streamGenerateContent, ";
        
        if (s.length() > 2)
            s.resize(s.length() - 2);
        
        return "[" + s + "]";
    }


    ModelInfo ModelInfo::fromJson(const nlohmann::json& j)
    {
        ModelInfo info;
        if(j.contains("name"))
            info.name = j.value("name", "");
        if(j.contains("version"))
            info.version = j.value("version", "");
        if(j.contains("displayName"))
            info.displayName = j.value("displayName", "");
        if(j.contains("description"))
            info.description = j.value("description", "");
            
        info.inputTokenLimit = j.value("inputTokenLimit", 0);
        info.outputTokenLimit = j.value("outputTokenLimit", 0);
        info.temperature = j.value("temperature", 0.0);
        info.topP = j.value("topP", 0.0);
        info.topK = j.value("topK", 0);

        info.supportedGenerationMethods = GM_NONE;
        if(j.contains("supportedGenerationMethods"))
        {
            for(const auto& methodJson : j["supportedGenerationMethods"])
                info.supportedGenerationMethods |= GenerationMethodHelper::fromString(methodJson.get<std::string>());
        }
        return info;
    }

    std::string ModelInfo::toString() const
    {
        return "Model: " + displayName + " (" + name + ")\n" +
               "Desc: " + description + "\n" +
               "Tokens: In=" + std::to_string(inputTokenLimit) + ", Out=" + std::to_string(outputTokenLimit) + "\n" +
               "Methods: " + GenerationMethodHelper::bitmaskToString(supportedGenerationMethods);
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

    TokenCountResponse TokenCountResponse::fromJson(const nlohmann::json& j)
    {
        return { j.value("totalTokens", 0) };
    }

    CachedContent CachedContent::fromJson(const nlohmann::json& j)
    {
        CachedContent c{};
        if(j.contains("name")) c.name = j.value("name", "");
        if(j.contains("model")) c.model = j.value("model", "");
        if(j.contains("displayName")) c.displayName = j.value("displayName", "");
        if(j.contains("createTime")) c.createTime = j.value("createTime", "");
        if(j.contains("updateTime")) c.updateTime = j.value("updateTime", "");
        if(j.contains("expireTime")) c.expireTime = j.value("expireTime", "");
        if(j.contains("ttl")) c.ttl = j.value("ttl", "");
        if(j.contains("usageMetadata")) c.usage.tokenCount = j["usageMetadata"].value("totalTokenCount", 0);
        
        return c;
    }

    nlohmann::json CachedContent::toJson() const
    {
        nlohmann::json j;
        j["model"] = model;
        if(!displayName.empty()) j["displayName"] = displayName;
        if(!ttl.empty()) j["ttl"] = ttl; // Ex.: "300s"
            
        if(systemInstruction.has_value())
        {
            j["systemInstruction"] = { {"parts", {{ {"text", systemInstruction.value()} }}} };
        }
            
        if(!contents.empty())
        {
            nlohmann::json contentArr = nlohmann::json::array();
            for(const auto& c : contents) contentArr.push_back(c.toJson());
            j["contents"] = contentArr;
        }
        
        if(!tools.empty())
        {
            nlohmann::json toolsArr = nlohmann::json::array();
            for(const auto& t : tools) toolsArr.push_back(t.toJson());
            j["tools"] = toolsArr;
        }

        return j;
    }

    ListCachedContentsResponse ListCachedContentsResponse::fromJson(const nlohmann::json& j)
    {
        ListCachedContentsResponse r;
        if(j.contains("cachedContents") && j["cachedContents"].is_array())
        {
            for(const auto& item : j["cachedContents"])
            {
                r.cachedContents.push_back(CachedContent::fromJson(item));
            }
        }
        if(j.contains("nextPageToken")) r.nextPageToken = j.value("nextPageToken", "");
        
        return r;
    }
}
