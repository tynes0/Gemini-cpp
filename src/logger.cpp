#include "gemini/logger.h"

namespace GeminiCPP
{
    std::shared_ptr<dtlog::logger<>> Log::logger_;

    void Log::init()
    {
        if (!logger_)
        {
            logger_ = std::make_shared<dtlog::logger<>>("Gemini");
            logger_->set_pattern("[%T] %N: %V%n");
        }
    }

    std::shared_ptr<dtlog::logger<>> Log::getLogger()
    {
        if (!logger_)
            init();
        
        return logger_;
    }

} // namespace GeminiCPP