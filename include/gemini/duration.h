#pragma once

#ifndef GEMINI_DURATION_H
#define GEMINI_DURATION_H

#include <cstdint>
#include <string>
#include <nlohmann/json_fwd.hpp>

namespace GeminiCPP
{
    /**
     * @brief Represents a duration of time with nanosecond precision.
     * * This structure roughly corresponds to `google.protobuf.Duration`.
     * * It stores time as signed seconds and signed nanoseconds.
     */
    struct Duration
    {
        /// @brief Signed seconds of the span of time. Must be from -315,576,000,000 to +315,576,000,000 inclusive.
        int64_t seconds = 0;
        
        /// @brief Signed fractions of a second at nanosecond resolution of the span of time.
        /// @note Durations less than one second are represented with a 0 `seconds` field and a positive or negative `nanos` field. For durations of one second or more, a non-zero value for the `nanos` field must be of the same sign as the `seconds` field.
        int32_t nanos = 0; // Range: -999,999,999 to +999,999,999

        /**
         * @brief Assigns the duration from a string representation.
         * @param str A string representing the duration (e.g., "3.5s").
         * @return Reference to this Duration object.
         */
        Duration& operator=(const std::string& str);

        // --- Factory Methods ---

        /**
         * @brief Creates a Duration object from a given number of seconds.
         * @param s The number of seconds.
         * @return A new Duration instance.
         */
        [[nodiscard]] static Duration fromSeconds(int64_t s);

        /**
         * @brief Creates a Duration object from a given number of milliseconds.
         * @param ms The number of milliseconds.
         * @return A new Duration instance.
         */
        [[nodiscard]] static Duration fromMillis(int64_t ms);

        /**
         * @brief Creates a Duration object from a given number of minutes.
         * @param m The number of minutes.
         * @return A new Duration instance.
         */
        [[nodiscard]] static Duration fromMinutes(int64_t m);

        /**
         * @brief Parses a string representation (e.g., "3.5s") into a Duration object.
         * @param str The string to parse.
         * @return A new Duration instance.
         */
        [[nodiscard]] static Duration fromString(const std::string& str);
        
        /**
         * @brief Deserializes a Duration from a JSON object (or string value within JSON).
         * @param j The JSON object/value.
         * @return A new Duration instance.
         */
        [[nodiscard]] static Duration fromJson(const nlohmann::json& j);
        
        /**
         * @brief Serializes the Duration to a JSON representation.
         * @return The JSON object (typically a string like "3.5s").
         */
        [[nodiscard]] nlohmann::json toJson() const;
        
        /**
         * @brief Converts the duration to its string representation (e.g., "3.5s").
         * @return The formatted string.
         */
        [[nodiscard]] std::string str() const;
    };
}

#endif // GEMINI_DURATION_H