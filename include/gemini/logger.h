#pragma once

#ifndef GEMINI_LOGGER_H
#define GEMINI_LOGGER_H

#include <memory>
#include "dtlog.h"

namespace GeminiCPP
{
    /**
     * @brief Singleton wrapper for the internal logging system (dtlog).
     * * Provides a central access point to configure and retrieve the logger instance.
     */
    class Log
    {
    public:
        /**
         * @brief Initializes the logger with default patterns.
         * * This is called automatically by getLogger() if not called explicitly.
         */
        static void init();

        /**
         * @brief Retrieves the shared logger instance.
         * @return std::shared_ptr to the dtlog logger.
         */
        [[nodiscard]] static std::shared_ptr<dtlog::logger<>> getLogger();

    private:
        // Singleton instance
        static std::shared_ptr<dtlog::logger<>> logger_;
    };

} // namespace GeminiCPP

/**
 * @brief Log a trace message.
 */
#define GEMINI_TRACE(...)    ::GeminiCPP::Log::getLogger()->trace(__VA_ARGS__)

/**
 * @brief Log an informational message.
 */
#define GEMINI_INFO(...)     ::GeminiCPP::Log::getLogger()->info(__VA_ARGS__)

/**
 * @brief Log a debug message.
 */
#define GEMINI_DEBUG(...)   ::GeminiCPP::Log::getLogger()->debug(__VA_ARGS__)

/**
 * @brief Log a warning message.
 */
#define GEMINI_WARN(...)     ::GeminiCPP::Log::getLogger()->warning(__VA_ARGS__)

/**
 * @brief Log an error message.
 */
#define GEMINI_ERROR(...)    ::GeminiCPP::Log::getLogger()->error(__VA_ARGS__)

/**
 * @brief Log a critical error message.
 */
#define GEMINI_CRITICAL(...) ::GeminiCPP::Log::getLogger()->critical(__VA_ARGS__)

#endif // GEMINI_LOGGER_H