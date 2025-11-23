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

    nlohmann::json Tool::toJson() const
    {
        nlohmann::json funcs = nlohmann::json::array();
        for(const auto& f : functionDeclarations)
            funcs.push_back(f.toJson());
            
        return {
            {"function_declarations", funcs}
        };
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
        return !text.empty();
    }

    bool Part::isBlob() const
    {
        return !inlineData.empty();
    }

    bool Part::isFunctionCall() const
    {
        return functionCall.has_value();
    }

    bool Part::isFunctionResponse() const
    {
        return functionResponse.has_value();
    }

    Part Part::Text(std::string t)
    {
        return {std::move(t), "", "", std::nullopt, std::nullopt};
    }

    Part Part::Media(const std::string& filepath, const std::string& customMimeType)
    {
        Part p;
        p.mimeType = customMimeType.empty() ? Utils::getMimeType(filepath) : customMimeType;
        p.inlineData = Utils::fileToBase64(filepath);
        return p;
    }

    Part Part::Call(FunctionCall call)
    {
        Part p;
        p.functionCall = std::move(call);
        return p;
    }

    Part Part::Response(FunctionResponse resp)
    {
        Part p;
        p.functionResponse = std::move(resp);
        return p;
    }

    nlohmann::json Part::toJson() const
    {
        if (isBlob())
        {
            return {
                    {"inlineData", {
                    {"mimeType", mimeType},
                    {"data", inlineData}
                    }}
            };
        }
        if (isFunctionCall())
        {
            return { {"functionCall", functionCall->toJson()} };
        }
        if (isFunctionResponse())
        {
            return { {"functionResponse", functionResponse->toJson()} };
        }
            
        return { {"text", text} };
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
                Part p;
                if (item.contains("text")) {
                    p.text = item["text"].get<std::string>();
                }
                else if (item.contains("inlineData"))
                {
                    p.mimeType = item["inlineData"]["mimeType"].get<std::string>();
                    p.inlineData = item["inlineData"]["data"].get<std::string>();
                }
                else if (item.contains("functionCall"))
                {
                    auto fc = item["functionCall"];
                    p.functionCall = {
                        fc["name"].get<std::string>(),
                        fc["args"]
                    };
                }
                c.parts.push_back(p);
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

    FinishReason FinishReasonHelper::fromString(const std::string& reason)
    {
        auto result = frenum::cast<FinishReason>(reason);
        return result.has_value() ? result.value() : FinishReason::FINISH_REASON_UNSPECIFIED;
    }

    std::string FinishReasonHelper::toString(FinishReason reason)
    {
        return frenum::to_string(reason);
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

        if(j.contains("supportedGenerationMethods"))
        {
            for(const auto& method : j["supportedGenerationMethods"])
            {
                info.supportedGenerationMethods.push_back(method.get<std::string>());
            }
        }
        return info;
    }

    std::string ModelInfo::toString() const
    {
        std::string methods = "[";
        for(const auto& m : supportedGenerationMethods)
            methods += m + ", ";
            
        if(methods.length() > 1)
            methods.resize(methods.length()-2);
            
        methods += "]";

        return "Model: " + displayName + " (" + name + ")\n" +
               "Desc: " + description + "\n" +
               "Tokens: In=" + std::to_string(inputTokenLimit) + ", Out=" + std::to_string(outputTokenLimit) + "\n" +
               "Methods: " + methods;
    }
}
