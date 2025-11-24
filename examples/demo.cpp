#include <iostream>
#include <vector>
#include <string>

#include "gemini/client.h"
#include "gemini/coroutine.h"
#include "gemini/logger.h"
#include "gemini/storage.h"

#ifdef _WIN32
#include <windows.h>
#endif

GeminiCPP::Task runMyLogic(GeminiCPP::Client& client) {

    auto futureGen = co_await client.request()
   .text("Tell me a long story.")
   .temperature(0.7)
   .generateAsync();

    if (futureGen)
        std::cout << futureGen.text();
}

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
    
    GeminiCPP::Log::init();

    std::string apiKey = std::getenv("GEMINI_API_KEY") ? std::getenv("GEMINI_API_KEY") : "";

    GeminiCPP::Client client(apiKey);

    auto response = client.request()
    .text("Tel me a long story!")
    .generate();
    
    if (response)
        std::cout << response.text();
}