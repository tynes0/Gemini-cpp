#include <iostream>
#include <vector>
#include <string>

#include "gemini/cache_registry.h"
#include "gemini/client.h"
#include "gemini/coroutine.h"
#include "gemini/logger.h"
#include "gemini/storage.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
    
    GeminiCPP::Log::init();

    std::string apiKey = std::getenv("GEMINI_API_KEY") ? std::getenv("GEMINI_API_KEY") : "";

    GeminiCPP::Client client(apiKey);
}