#include "gemini/client.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "gemini/model.h"

namespace GeminiCPP
{
    Client::Client(std::string api_key) : api_key_(std::move(api_key)) {}

    std::string Client::generateContent(const std::string& prompt, std::string_view model_id)
    {
        std::string url = root_url_ + std::string(model_id) + ":generateContent";

        nlohmann::json payload =
            {
            {"contents", {
                    {
                        {"parts", {
                            {{"text", prompt}}
                        }}
                    }
            }}
        };

        cpr::Response r = cpr::Post(
            cpr::Url{url},
            cpr::Header{
                {"Content-Type", "application/json"},
                {"x-goog-api-key", api_key_}
            },
            cpr::Body{payload.dump()}
        );

        if (r.status_code != 200)
        {
            return "API Error (" + std::to_string(r.status_code) + "): " + r.text;
        }

        auto json_response = nlohmann::json::parse(r.text);
        try
        {
            return json_response["candidates"][0]["content"]["parts"][0]["text"].get<std::string>();
        }
        catch (...)
        {
            return "Response could not parse!";
        }
    }

    std::string Client::generateContent(const std::string& prompt, Model model)
    {
        return generateContent(prompt, modelStringRepresentation(model));
    }

}