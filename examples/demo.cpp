#include <iostream>

#include "gemini/client.h"
#include "gemini/logger.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
    GeminiCPP::Log::init();
    GeminiCPP::Client client("API_KEY");


    return 0;
}