#pragma once

#ifndef GEMINI_LOGGER_H
#define GEMINI_LOGGER_H

#include <memory>
#include "dtlog.h"

namespace GeminiCPP
{
    class Log
    {
    public:
        static void init();

        static std::shared_ptr<dtlog::logger<>> getLogger();

    private:
        // Singleton instance
        static std::shared_ptr<dtlog::logger<>> logger_;
    };

} // namespace GeminiCPP

#define GEMINI_TRACE(...)    ::GeminiCPP::Log::getLogger()->trace(__VA_ARGS__)
#define GEMINI_INFO(...)     ::GeminiCPP::Log::getLogger()->info(__VA_ARGS__)
#define GEMINI_WARN(...)     ::GeminiCPP::Log::getLogger()->warning(__VA_ARGS__)
#define GEMINI_ERROR(...)    ::GeminiCPP::Log::getLogger()->error(__VA_ARGS__)
#define GEMINI_CRITICAL(...) ::GeminiCPP::Log::getLogger()->critical(__VA_ARGS__)

#endif // GEMINI_LOGGER_H