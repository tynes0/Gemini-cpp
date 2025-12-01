#include "gemini/types/batch_api_types.h"

namespace GeminiCPP
{
    Operation Operation::fromJson(const nlohmann::json& j)
    {
        return Operation{};
    }

    nlohmann::json Operation::toJson() const
    {
        return nlohmann::json{};
    }
}
