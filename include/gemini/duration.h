#pragma once

#ifndef GEMINI_DURATION_H
#define GEMINI_DURATION_H

#include <cstdint>
#include <string>
#include <nlohmann/json_fwd.hpp>

namespace GeminiCPP
{
    // Ref: https://protobuf.dev/reference/protobuf/google.protobuf/#duration
    struct Duration
    {
        int64_t seconds = 0;
        int32_t nanos = 0; // -999,999,999 i +999,999,999

        [[nodiscard]] static Duration fromSeconds(int64_t s);
        [[nodiscard]] static Duration fromMillis(int64_t ms);
        [[nodiscard]] static Duration fromMinutes(int64_t m);

        [[nodiscard]] static Duration fromString(const std::string& str);
        [[nodiscard]] static Duration fromJson(const nlohmann::json& j);
        
        [[nodiscard]] nlohmann::json toJson() const;
        [[nodiscard]] std::string toString() const;
    };
}

#endif // GEMINI_DURATION_H