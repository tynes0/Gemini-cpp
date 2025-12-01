#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

#include "gemini/client.h"
#include "gemini/logger.h"
#include "gemini/request_builder.h"
#include "gemini/chat_session.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define SEPERATOR(x) "\n\033[1;35m========================================\033[0m\n\033[1;36m " << (x) << " \033[0m\n\033[1;35m========================================\033[0m\n"


int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
    
    GeminiCPP::Log::init();
    GEMINI_INFO("Gemini C++ Client Demo...");

    std::string apiKey = std::getenv("GEMINI_API_KEY") ? std::getenv("GEMINI_API_KEY") : "";
    if(apiKey.empty())
    {
        GEMINI_CRITICAL("Error: GEMINI_API_KEY environment not found!");
        return 1;
    }

    GeminiCPP::Client client(apiKey);
    
    return 0;
}