#pragma once

#ifndef GEMINI_UUID_H
#define GEMINI_UUID_H

#include <string>

namespace GeminiCPP
{
    class Uuid
    {
    public:
        static std::string generate();
    };
}

#endif // GEMINI_UUID_H