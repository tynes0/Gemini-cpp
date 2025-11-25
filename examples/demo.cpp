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
    GeminiCPP::CacheRegistry registry("my_caches.json");

    std::string alias = "Project_Alpha_Docs";

    auto cacheIdOpt = registry.getCacheId(alias);

    if (cacheIdOpt) {
        std::string cacheId = cacheIdOpt.value();
        std::cout << "Local record found! ID: " << cacheId << "\n";
        
        auto check = client.getCachedContent(cacheId);
        if (!check) {
            std::cout << "But it has expired or been deleted. The record is being deleted...\n";
            registry.unregisterCache(alias);
        } else {
            std::cout << "Cache is active! Being used...\n";
            auto chat = client.startChat();
            chat.setCachedContent(cacheId);
            // ...
            return 0;
        }
    }

    std::cout << "Cache not found, creating a new one...\n";
    
    GeminiCPP::CachedContent cc;
    cc.model = GeminiCPP::ResourceName(GeminiCPP::Model::GEMINI_2_5_FLASH);
    cc.displayName = "Project Alpha Documentation";
    cc.ttl = "120s"; // 2 min
    cc.contents.push_back(
        GeminiCPP::Content::User().file(R"(C:\Users\cihan\Desktop\file.pdf)")
    );

    auto createRes = client.createCachedContent(cc);
    
    if (createRes) {
        std::cout << "Cache created: " << createRes->name << "\n";
        
        registry.registerCache(alias, *createRes);
        std::cout << "Added to the registry as '" << alias << "'\n";
    } else {
        std::cout << "Error: " << createRes.errorMessage << "\n";
    }

    return 0;
}