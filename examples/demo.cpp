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

    std::cout << SEPERATOR("TEST 1: API Key & Connection check");
    auto verifyRes = client.verifyApiKey();
    if(verifyRes.isValid)
    {
        GEMINI_INFO("Successfully connected! Status code: {}", frenum::value(verifyRes.statusCode));
    }
    else
    {
        GEMINI_ERROR("Connection failed: {}", verifyRes.message);
        return 1;
    }

    std::cout << SEPERATOR("TEST 2: Models API - List & Get");
    
    GEMINI_TRACE("Listing models...");
    auto modelsRes = client.models.list(1000);
    
    if(modelsRes.success)
    {
        GEMINI_INFO("{} models found.", modelsRes->models.size());
        if(!modelsRes->models.empty())
        {
            for(size_t i = 0; i <  modelsRes->models.size(); ++i)
            {
                const auto& m = modelsRes->models[i];
                GEMINI_DEBUG("Model [{}]: {} ({})", i, m.displayName, m.name.str());
            }
        }

        GEMINI_TRACE("getting gemini-2.5-flash details...");
        auto modelInfo = client.models.get("models/gemini-2.5-flash");
        if(modelInfo.success)
        {
            GEMINI_INFO("Model Detail: {} - Max Input Token: {}", modelInfo->displayName, modelInfo->inputTokenLimit);
        }
        else
        {
            GEMINI_WARN("Getting model detail failed: {}", modelInfo.errorMessage);
        }
    }
    else
    {
        GEMINI_ERROR("Listing models failed: {}", modelsRes.errorMessage);
    }

    std::cout << SEPERATOR("TEST 3: Tokens API - Token Counting");
    
    std::string sampleText = "Hi Gemini, This is a token counting test. C++ is wonderful language.";
    GEMINI_TRACE("Counting tokens: '{}'", sampleText);
    
    auto tokenRes = client.tokens.count("gemini-2.0-flash", sampleText);
    if(tokenRes.success)
    {
        GEMINI_INFO("Calculated token count: {}", tokenRes->totalTokens);
    }
    else
    {
        GEMINI_ERROR("Token counting failed: {}", tokenRes.errorMessage);
    }

    std::cout << SEPERATOR("TEST 4: Generate Content (Sync)");
    
    GEMINI_DEBUG("Generating request (Model: gemini-2.0-flash)...");
    auto response = client.request()
        .model("gemini-2.0-flash")
        .text("Tell me something about C++.")
        .temperature(0.7f)
        .generate();

    if(response.success)
    {
        GEMINI_INFO("Gemini's answer: \033[1;32m{}\033[0m", response.text());
        GEMINI_DEBUG("Used tokens: Input={}, Output={}", response.inputTokens, response.outputTokens);
    }
    else
    {
        GEMINI_ERROR("Generation Error: {}", response.errorMessage);
    }

    std::cout << SEPERATOR("TEST 5: Generate Content (Stream)");
    
    GEMINI_DEBUG("Stream request sending...");
    std::cout << "\033[1;33mStream Output: \033[0m";
    
    auto streamRes = client.request()
        .model("gemini-2.0-flash")
        .text("Count from 1 to 100 and add a fruit next to the number as you count.")
        .stream([](std::string_view chunk) {
            std::cout << chunk << std::flush;
        });
    
    std::cout << "\n";
    
    if(streamRes.success)
    {
        GEMINI_INFO("Stream completed.");
    }
    else
    {
        GEMINI_ERROR("Stream error: {}", streamRes.errorMessage);
    }

    std::cout << SEPERATOR("TEST 6: Chat Session");
    
    auto chat = client.startChat("gemini-2.0-flash");

    std::string message = "Hi, My name is Tynes0. I am a C++ developer.";
    GEMINI_TRACE("User: {}", message);
    auto chatResp1 = chat.send(message);
    GEMINI_INFO("Gemini: {}", chatResp1.text());

    std::string message2 = "What is my name and job?";
    GEMINI_TRACE("User: {}", message2);
    auto chatResp2 = chat.send(message2);
    
    if(chatResp2.success)
    {
        GEMINI_INFO("Gemini: {}", chatResp2.text());
    }
    else
    {
        GEMINI_ERROR("Chat error: {}", chatResp2.errorMessage);
    }

    std::cout << SEPERATOR("TEST 7: Files API");

    std::string tempFileName = "test_gemini_cpp.txt";
    {
        std::ofstream tempFile(tempFileName);
        tempFile << "This file created by Gemini C++ lib for testing.";
        tempFile.close();
    }
    GEMINI_DEBUG("Temp file created: {}", tempFileName);

    // 1. Upload
    GEMINI_TRACE("Uploading file...");
    auto uploadRes = client.files.upload(tempFileName, "GeminiCPP Test File");
    
    if(uploadRes.success)
    {
        std::string fileUri = uploadRes->uri;
        std::string fileName = uploadRes->name.str();
        GEMINI_INFO("File uploaded! URI: {}", fileUri);
        GEMINI_DEBUG("Server file name (ID): {}", fileName);

        GEMINI_TRACE("Querying file metadata...");
        auto getFileRes = client.files.get(fileName);
        if(getFileRes.success)
        {
            GEMINI_INFO("File status: {}", frenum::to_string(getFileRes->state));
        }

        GEMINI_TRACE("Deleting file...");
        auto deleteRes = client.files.deleteFile(fileName);
        if(deleteRes.success)
        {
            GEMINI_INFO("File deleted successfully.");
        }
        else
        {
            GEMINI_ERROR("Deleting error: {}", deleteRes.errorMessage);
        }
    }
    else
    {
        GEMINI_ERROR("Uploading failed: {}", uploadRes.errorMessage);
    }

    std::filesystem::remove(tempFileName);

    std::cout << SEPERATOR("TEST 8: Async Generation (Future/Await)");
    
    GEMINI_DEBUG("Started Async request...");
    auto futureRes = client.request()
        .text("Tell me something about async programming")
        .generateAsync();

    GEMINI_TRACE("Processing request...");

    auto asyncResult = futureRes.get();
    if(asyncResult.success)
    {
        GEMINI_INFO("Async answer: {}", asyncResult.text());
    }
    else
    {
        GEMINI_ERROR("Async Error: {}", asyncResult.errorMessage);
    }

    std::cout << SEPERATOR("TEST CASES COMPLETED!");
    return 0;
}