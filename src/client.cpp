#include "gemini/client.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "gemini/logger.h"
#include "gemini/model.h"
#include "gemini/http_status.h"

namespace GeminiCPP
{
    Client::Client(std::string api_key) : api_key_(std::move(api_key)) {}

    RequestBuilder Client::request()
    {
        return RequestBuilder(this);
    }

    ChatSession Client::startChat(Model model)
    {
        return { this, model };
    }
    
    GenerationResult Client::generateContent(const std::string& prompt, std::string_view model_id)
    {
        std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + std::string(model_id) + ":generateContent";

        nlohmann::json payload = {
            {"contents", {{
                {"parts", {{{"text", prompt}}}}
            }}}
        };

        GenerationResult rawResult = submitRequest(url, payload);
    }

    GenerationResult Client::generateContent(const std::string& prompt, Model model)
    {
        return generateContent(prompt, modelStringRepresentation(model));
    }

GenerationResult Client::submitRequest(const std::string& url, const nlohmann::json& payload)
{
    cpr::Response r = cpr::Post(
        cpr::Url{url},
        cpr::Header{
            {"Content-Type", "application/json"},
            {"x-goog-api-key", api_key_}
        },
        cpr::Body{payload.dump()}
    );

    if (!HttpStatusHelper::isSuccess(r.status_code))
    {
        std::string errorMsg = r.text;
        try {
            auto jsonErr = nlohmann::json::parse(r.text);
            if(jsonErr.contains("error"))
                errorMsg = jsonErr["error"]["message"].get<std::string>();
        } catch(...) {}

        GEMINI_ERROR("API Error [{}]: {}", r.status_code, errorMsg);
        return GenerationResult::Failure(errorMsg, r.status_code);
    }

    try
    {
        auto json_response = nlohmann::json::parse(r.text);
        
        if (!json_response.contains("candidates") || json_response["candidates"].empty())
        {
            if (json_response.contains("promptFeedback"))
            {
                 return GenerationResult::Failure("Blocked by Safety Filter", 200);
            }
            return GenerationResult::Failure("No candidates returned", 200);
        }

        auto candidate = json_response["candidates"][0];
        
        if (candidate.contains("content"))
        {
             Content content = Content::fromJson(candidate["content"]);
             
             return GenerationResult::Success(content, r.status_code);
        }
        
        return GenerationResult::Failure("Candidate has no content", 200);
    }
    catch (const std::exception& e)
    {
        GEMINI_ERROR("JSON Parse Error: {}", e.what());
        return GenerationResult::Failure("Response parse error", r.status_code);
    }
}

    GenerationResult Client::generateFromBuilder(Model model, const std::string& sys_instr, const std::vector<Part>& parts)
    {
        nlohmann::json partsJson = nlohmann::json::array();
        for (const auto& p : parts)
            partsJson.push_back(p.toJson());
        
        nlohmann::json payload = {
            {"contents", {{
                {"role", "user"},
                {"parts", partsJson}
            }}}
        };

        if (!sys_instr.empty())
        {
            payload["system_instruction"] = {
                {"parts", {{ {"text", sys_instr} }}}
            };
        }

        std::string url = "https://generativelanguage.googleapis.com/v1beta/models/"
        + std::string(modelStringRepresentation(model)) + ":generateContent";

        GenerationResult rawResult = submitRequest(url, payload);
    }
}
