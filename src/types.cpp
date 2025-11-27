#include "gemini/types.h"

#include "gemini/base.h"
#include "gemini/utils.h"

namespace GeminiCPP
{
    FunctionCallingConfig FunctionCallingConfig::fromJson(const nlohmann::json& j)
    {
        FunctionCallingConfig result{};

        if (j.contains("mode"))
            result.mode = frenum::cast<FunctionCallingMode>(j["mode"]);

        if (j.contains("allowedFunctionNames"))
        {
            result.allowedFunctionNames = std::vector<std::string>{};
            result.allowedFunctionNames->reserve(j["allowedFunctionNames"].size());
            for (const auto& name : j["allowedFunctionNames"])
                result.allowedFunctionNames->push_back(name);
        }

        return result;
    }

    nlohmann::json FunctionCallingConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (mode.has_value())
            j["mode"] = frenum::to_string(mode.value());

        if (allowedFunctionNames.has_value())
        {
            j["allowedFunctionNames"] = nlohmann::json::array();
            for (const auto& name : *allowedFunctionNames)
                j["allowedFunctionNames"].push_back(name);
        }
        return j;
    }

    LatLng LatLng::fromJson(const nlohmann::json& j)
    {
        LatLng result;
        result.latitude = j.value("latitude", 0);
        result.longitude = j.value("longitude", 0);
        return result;
    }
    
    nlohmann::json LatLng::toJson() const
    {
        return {
            {"latitude", latitude},
            {"longitude", longitude}
        };
    }

    RetrievalConfig RetrievalConfig::fromJson(const nlohmann::json& j)
    {
        RetrievalConfig result{};

        if (j.contains("latLng"))
            result.latLng = LatLng::fromJson(j["latLng"]);

        if (j.contains("languageCode"))
            result.languageCode = j["languageCode"];
        
        return result;
    }
    
    nlohmann::json RetrievalConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        if (latLng.has_value())
            j["latLng"] = latLng->toJson();
        
        if (languageCode.has_value())
            j["languageCode"] = *languageCode;
        return j;
    }

    ToolConfig ToolConfig::fromJson(const nlohmann::json& j)
    {
        ToolConfig result{};

        if (j.contains("functionCallingConfig"))
            result.functionCallingConfig = FunctionCallingConfig::fromJson(j["functionCallingConfig"]);

        if (j.contains("retrievalConfig"))
            result.retrievalConfig = RetrievalConfig::fromJson(j["retrievalConfig"]);

        return result;
    }
    
    nlohmann::json ToolConfig::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (functionCallingConfig.has_value())
            j["functionCallingConfig"] = functionCallingConfig->toJson();
        
        if (retrievalConfig.has_value())
            j["retrievalConfig"] = retrievalConfig->toJson();
        
        return j;
    }

    FunctionDeclaration FunctionDeclaration::fromJson(const nlohmann::json& j)
    {
        FunctionDeclaration result{};

        result.name = j.value("name", "");
        result.description = j.value("description", "");

        if (j.contains("parameters"))
            result.parameters = j["parameters"];

        return result;
    }

    nlohmann::json FunctionDeclaration::toJson() const
    {
        return {
            {"name", name},
            {"description", description},
            {"parameters", parameters}
        };
    }

    GoogleSearch GoogleSearch::fromJson(const nlohmann::json& j)
    {
        GEMINI_UNUSED(j);
        return GoogleSearch{};
    }

    nlohmann::json GoogleSearch::toJson() const
    {
        return nlohmann::json::object();
    }

    GoogleMaps GoogleMaps::fromJson(const nlohmann::json& j)
    {
        GEMINI_UNUSED(j);
        return GoogleMaps{};
    }

    nlohmann::json GoogleMaps::toJson() const
    {
        if (enableWidget)
            return { {"enableWidget", true} };
        return nlohmann::json::object();
    }

    CodeExecution CodeExecution::fromJson(const nlohmann::json& j)
    {
        GEMINI_UNUSED(j);
        return CodeExecution{};
    }

    nlohmann::json CodeExecution::toJson() const
    {
        return nlohmann::json::object();
    }

    Tool Tool::fromJson(const nlohmann::json& j)
    {
        Tool result{};
        
        if (j.contains("FunctionDeclarations"))
        {
            result.functionDeclarations.reserve(j["FunctionDeclarations"].size());
            
            for (const auto& functionDeclaration : j["FunctionDeclarations"])
                result.functionDeclarations.push_back(FunctionDeclaration::fromJson(functionDeclaration));
        }

        if (j.contains("codeExecution"))
            result.codeExecution = CodeExecution::fromJson(j["codeExecution"]);

        if (j.contains("googleSearch"))
            result.googleSearch = GoogleSearch::fromJson(j["googleSearch"]);

        if (j.contains("googleMaps"))
            result.googleMaps = GoogleMaps::fromJson(j["googleMaps"]);

        return result;
    }

    nlohmann::json Tool::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
            
        if (!functionDeclarations.empty())
        {
            nlohmann::json funcs = nlohmann::json::array();
            for(const auto& f : functionDeclarations)
                funcs.push_back(f.toJson());
            
            j["functionDeclarations"] = funcs;
        }
            
        if (googleSearch.has_value())
        {
            j["googleSearch"] = googleSearch->toJson();
        }

        if (codeExecution.has_value())
        {
            j["codeExecution"] = codeExecution->toJson();
        }

        if (googleMaps.has_value())
        {
            j["googleMaps"] = googleMaps->toJson();
        }
            
        return j;
    }

    FunctionResponseBlob FunctionResponseBlob::fromJson(const nlohmann::json& j)
    {
        FunctionResponseBlob result;
        result.mimeType = j.value("mimeType", "");
        result.data = j.value("data", "");
        return result;
    }

    nlohmann::json FunctionResponseBlob::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        j["mimeType"] = mimeType;
        j["data"] = data;
        return j;
    }

    FunctionResponsePart FunctionResponsePart::fromJson(const nlohmann::json& j)
    {
        FunctionResponsePart part{};
        if (j.contains("inlineData"))
            part.data = FunctionResponseBlob::fromJson(j["inlineData"]);

        return part;
    }

    nlohmann::json FunctionResponsePart::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        
        std::visit([&j](const auto& arg)
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, FunctionResponseBlob>) j["inlineData"] = arg.toJson();
        }, data);

        return j;
    }

    TextData TextData::fromJson(const nlohmann::json& j)
    {
        TextData result{};
        
        if (j.is_string())
            result.text = j.get<std::string>();
        if (j.is_object())
            result.text = j.value("text", "");
        
        return result;
    }
    
    nlohmann::json TextData::toJson() const
    {
        nlohmann::json j = text;
        return j;
    }

    Blob Blob::createFromPath(const std::string& filepath, const std::string& customMimeType)
    {
        return Blob{
            .mimeType = customMimeType.empty() ? Utils::getMimeType(filepath) : customMimeType,
            .data = Utils::fileToBase64(filepath)
        };
    }

    Blob Blob::fromJson(const nlohmann::json& j)
    {
        Blob result;
        result.mimeType = j.value("mimeType", "");
        result.data = j.value("data", "");
        return result;
    }

    nlohmann::json Blob::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        j["mimeType"] = mimeType;
        j["data"] = data;
        return j;
    }

    FunctionCall FunctionCall::fromJson(const nlohmann::json& j)
    {
        FunctionCall result{};
        if (j.contains("id"))
            result.id = j["id"];

        result.name = j.value("name", "");
        result.args = j.value("args", nlohmann::json::object());

        return result;
    }
    
    nlohmann::json FunctionCall::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        j["name"] = name;
        j["args"] = args;
        if (id.has_value())
            j["id"] = id.value();
        return j;
    }
    
    FunctionResponse FunctionResponse::fromJson(const nlohmann::json& j)
    {
        FunctionResponse result;
        if (j.contains("id")) result.id = j["id"];
        if (j.contains("willContinue")) result.willContinue = j["willContinue"];
        if (j.contains("scheduling")) result.scheduling = frenum::cast<Scheduling>(j["scheduling"]);
        
        if (j.contains("parts"))
        {
            result.parts = std::vector<FunctionResponsePart>{};
            result.parts->reserve(j["parts"].size());
            
            for (const auto& p : j["parts"])
                result.parts->push_back(FunctionResponsePart::fromJson(p));
        }
        
        result.name = j.value("name", "");

        result.responseContent = j.contains("response")
            ? j["response"].value("content", nlohmann::json::object())
            : nlohmann::json::object();

        return result;
    }
    
    nlohmann::json FunctionResponse::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();

        if (id.has_value())
        {
            j["id"] = id.value();
        }
        
        j["name"] = name;

        j["response"] = {
            {"name", name},
            {"content", responseContent}
        };

        if (parts.has_value())
        {
            j["parts"] = nlohmann::json::array();
            for (const auto& p : *parts)
                j["parts"].push_back(p.toJson());
        }

        if (willContinue.has_value())
        {
            j["willContinue"] = willContinue.value();
        }

        if (scheduling.has_value())
        {
            j["scheduling"] = frenum::to_string(scheduling.value());
        }

        return j;
    }
    
    FileData FileData::fromJson(const nlohmann::json& j)
    {
        FileData result{};
        if (j.contains("mimeType"))
            result.mimeType = j["mimeType"];

        result.fileUri = j.value("fileUri", "");
        return result;
    }

    nlohmann::json FileData::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        if (mimeType.has_value())
            j["mimeType"] = *mimeType;

        j["fileUri"] = fileUri;
        return j;
    }

    ExecutableCode ExecutableCode::fromJson(const nlohmann::json& j)
    {
        return {
            .language = frenum::cast<Language>(j.value("language", "PYTHON")).value_or(Language::PYTHON),
            .code = j.value("code", "")
        };
    }

    nlohmann::json ExecutableCode::toJson() const
    {
        return {
            {"language", frenum::to_string(language)},
            {"code", code}
        };
    }

    CodeExecutionResult CodeExecutionResult::fromJson(const nlohmann::json& j)
    {
        return {
            .outcome = frenum::cast<Outcome>(j.value("outcome", "")).value_or(Outcome::OUTCOME_UNSPECIFIED),
            .output = j.value("output", ""),
        };
    }

    nlohmann::json CodeExecutionResult::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        j["outcome"] = frenum::to_string(outcome);

        if (output.has_value())
            j["output"] = output.value();

        return j;
    }

    VideoMetadata VideoMetadata::fromJson(const nlohmann::json& j)
    {
        VideoMetadata result{};
        if (j.contains("startOffset"))
            result.startOffset = Duration::fromJson(j["startOffset"]);

        
        if (j.contains("endOffset"))
            result.endOffset = Duration::fromJson(j["endOffset"]);

        if (j.contains("fps"))
            result.fps = j["fps"];

        return result;
    }

    nlohmann::json VideoMetadata::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        
        if (startOffset.has_value()) j["startOffset"] = startOffset->toJson();
        if (endOffset.has_value()) j["endOffset"] = endOffset->toJson();
        if (fps.has_value()) j["fps"] = *fps;
        
        return j;
    }

    bool Part::isText() const
    {
        return std::holds_alternative<TextData>(data);
    }
    bool Part::isBlob() const
    {
        return std::holds_alternative<Blob>(data);
    }

    bool Part::isFunctionCall() const
    {
        return std::holds_alternative<FunctionCall>(data);
    }
    
    bool Part::isFunctionResponse() const
    {
        return std::holds_alternative<FunctionResponse>(data);
    }
    
    bool Part::isFileData() const
    {
        return std::holds_alternative<FileData>(data);
    }

    bool Part::isExecutableCode() const
    {
        return std::holds_alternative<ExecutableCode>(data);
    }

    bool Part::isCodeExecutionResult() const
    {
        return std::holds_alternative<CodeExecutionResult>(data);
    }

    const TextData* Part::getText() const
    {
        return std::get_if<TextData>(&data);
    }

    const Blob* Part::getBlob() const
    {
        return std::get_if<Blob>(&data);
    }

    const FunctionCall* Part::getFunctionCall() const
    {
        return std::get_if<FunctionCall>(&data);
    }

    const FunctionResponse* Part::getFunctionResponse() const
    {
        return std::get_if<FunctionResponse>(&data);
    }

    const FileData* Part::getFileData() const
    {
        return std::get_if<FileData>(&data);
    }

    const ExecutableCode* Part::getExecutableCode() const
    {
        return std::get_if<ExecutableCode>(&data);
    }

    const CodeExecutionResult* Part::getCodeExecutionResult() const
    {
        return std::get_if<CodeExecutionResult>(&data);
    }

    Part Part::Text(TextData t)
    {
        Part p;
        p.data = std::move(t);
        return p;
    }

    Part Part::Media(Blob blobData)
    {
        Part p;
        p.data = std::move(blobData);
        return p;
    }

    Part Part::Uri(FileData fileData)
    {
        Part p;
        p.data = std::move(fileData);
        return p;
    }

    Part Part::Call(FunctionCall call)
    {
        Part p;
        p.data = std::move(call);
        return p;
    }

    Part Part::Response(FunctionResponse resp)
    {
        Part p;
        p.data = std::move(resp);
        return p;
    }

    Part Part::Code(ExecutableCode code)
    {
        Part p;
        p.data = std::move(code);
        return p;
    }

    Part Part::ExecutionResult(CodeExecutionResult result)
    {
        Part p;
        p.data = std::move(result);
        return p;
    }

    Part Part::fromJson(const nlohmann::json& j)
    {
        Part result{};

        if (j.contains("text")) result.data = TextData::fromJson(j["text"]);
        else if (j.contains("inlineData")) result.data = Blob::fromJson(j["inlineData"]);
        else if (j.contains("functionCall")) result.data = FunctionCall::fromJson(j["functionCall"]);
        else if (j.contains("functionResponse")) result.data = FunctionResponse::fromJson(j["functionResponse"]);
        else if (j.contains("fileData")) result.data = FileData::fromJson(j["fileData"]);
        else if (j.contains("executableCode")) result.data = ExecutableCode::fromJson(j["executableCode"]);
        else if (j.contains("codeExecutionResult")) result.data = CodeExecutionResult::fromJson(j["codeExecutionResult"]);

        if (j.contains("thought"))
            result.thought = j["thought"];
        if (j.contains("thoughtSignature"))
            result.thoughtSignature = j["thoughtSignature"];
        result.partMetadata = j.value("partMetadata", nlohmann::json::object());

        if (j.contains("videoMetadata"))
            result.metadata = VideoMetadata::fromJson(j["videoMetadata"]);
        
        return result;
    }

    nlohmann::json Part::toJson() const
    {
        nlohmann::json j = nlohmann::json::object();
        
        std::visit([&j](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, TextData>) j["text"] = arg.text;
            else if constexpr (std::is_same_v<T, Blob>) j["inlineData"] = arg.toJson();
            else if constexpr (std::is_same_v<T, FileData>) j["fileData"] = arg.toJson();
            else if constexpr (std::is_same_v<T, FunctionCall>) j["functionCall"] = arg.toJson();
            else if constexpr (std::is_same_v<T, FunctionResponse>) j["functionResponse"] = arg.toJson();
            else if constexpr (std::is_same_v<T, ExecutableCode>) j["executableCode"] = arg.toJson();
            else if constexpr (std::is_same_v<T, CodeExecutionResult>) j["codeExecutionResult"] = arg.toJson();
        }, data);

        if (thought.has_value())
            j["thought"] = *thought;

        if (thoughtSignature.has_value())
            j["thoughtSignature"] = *thoughtSignature;

        j["partMetadata"] = partMetadata;
        
        std::visit([&j](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, VideoMetadata>) j["videoMetadata"] = arg.toJson();
        }, metadata);

        return j;
    }

    Content Content::User()
    {
        return {
            .role = Role::USER,
            .parts = {}
        };
    }

    Content Content::Model()
    {
        return {
            .role = Role::MODEL,
            .parts = {}
        };
    }

    Content Content::Function()
    {
        return {
            .role = Role::FUNCTION,
            .parts = {}
        };
    }

    Content& Content::text(const std::string& t)
    {
        parts.push_back(Part::Text(TextData{ .text = t }));
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
        FileData data{
            .mimeType = mimeType,
            .fileUri = uri,
        };
        
        parts.push_back(Part::Uri(data));
        return *this;
    }

    Content& Content::media(const std::string& filepath, const std::string& mimeType)
    {
        parts.push_back(Part::Media(Blob::createFromPath(filepath, mimeType)));
        return *this;
    }

    Content& Content::functionResponse(FunctionResponse response)
    {
        parts.push_back(Part::Response(std::move(response)));
        role = Role::FUNCTION; 
        return *this;
    }

    nlohmann::json Content::toJson() const
    {
        std::string roleStr = "user";
        if (role == Role::MODEL)
            roleStr = "model";
        
        nlohmann::json partsJson = nlohmann::json::array();
        
        for (const auto& p : parts)
            partsJson.push_back(p.toJson());
            
        return {
            {"role", roleStr},
            {"parts", partsJson}
        };
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
            c.parts.reserve(j["parts"].size());
            for (const auto& item : j["parts"])
                c.parts.push_back(Part::fromJson(item));
        }
        
        return c;
    }

    nlohmann::json ThinkingConfig::toJson() const
    {
        nlohmann::json j;
        
        j["includeThoughts"] = includeThoughts;

        if (thinkingBudget.has_value())
        {
            j["thinkingBudget"] = thinkingBudget.value();
        }

        if (thinkingLevel.has_value())
        {
            switch (thinkingLevel.value())
            {
            case ThinkingLevel::LOW:  j["thinkingLevel"] = "low"; break;
            case ThinkingLevel::HIGH: j["thinkingLevel"] = "high"; break;
            }
        }

        return j;
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
        if (thinkingConfig.has_value())   j["thinkingConfig"] = thinkingConfig->toJson();

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
        if(j.contains("ttl")) c.ttl = Duration::fromJson(j["ttl"]);
        if(j.contains("usageMetadata")) c.usage.tokenCount = j["usageMetadata"].value("totalTokenCount", 0);
        
        return c;
    }

    nlohmann::json CachedContent::toJson() const
    {
        nlohmann::json j;
        j["model"] = model;
        if(!displayName.empty())
        {
            j["displayName"] = displayName;
        }
        
        if(!ttl.has_value())
        {
            j["ttl"] = ttl->toJson();
        }
        
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
