#pragma once

#ifndef GEMINI_UUID_H
#define GEMINI_UUID_H

#include <string>

namespace GeminiCPP
{
    /**
     * @brief Utility for generating Unique IDs.
     */
    class Uuid
    {
    public:
        /**
         * @brief Generates a random UUID (Version 4).
         * @return A UUID string (e.g., "550e8400-e29b-41d4-a716-446655440000").
         */
        static std::string generate();
    };
}

#endif // GEMINI_UUID_H