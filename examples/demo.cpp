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
    auto chat = client.startChat(GeminiCPP::Model::GEMINI_2_0_FLASH);
    
    chat.registerFunction(
        "topla", 
        [](int a, int b) { 
            std::cout << ">> C++: Toplama yapiliyor: " << a << " + " << b << "\n";
            return a + b; 
        }, 
        "Iki tamsayiyi toplar.", 
        {"a", "b"}
    );

    chat.registerFunction(
        "hava_durumu",
        [](std::string sehir) -> std::string {
            std::cout << ">> C++: Hava durumu sorgulaniyor: " << sehir << "\n";
            if(sehir == "Ankara") return "Soguk, 5 derece";
            return "Gunesli, 25 derece";
        },
        "Bir sehrin hava durumunu verir.",
        {"sehir"}
    );

    std::cout << "Ben: Ankara'da hava nasil ve 5 ile 10'u toplarsan kac eder?\n";
    
    auto res = chat.send("Ankara'da hava nasil ve 5 ile 10'u toplarsan kac eder?");

    if (res) {
        std::cout << "Gemini: " << res.text() << "\n";
    } else {
        std::cout << "Hata: " << res.errorMessage << "\n";
    }

    return 0;
}