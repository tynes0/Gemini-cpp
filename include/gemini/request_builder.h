#pragma once

#ifndef GEMINI_REQUEST_BUILDER_H
#define GEMINI_REQUEST_BUILDER_H

#include <future>
#include <string>
#include <vector>
#include "types.h"
#include "model.h"
#include "response.h"

namespace GeminiCPP
{
    class Client;

    class RequestBuilder
    {
    public:
        explicit RequestBuilder(Client* client);

        RequestBuilder& model(Model m);
        RequestBuilder& systemInstruction(const std::string& instruction);
        RequestBuilder& text(const std::string& t);
        RequestBuilder& image(const std::string& filepath);
        RequestBuilder& googleSearch();

        // --- GENERATION CONFIG METHODS ---
        RequestBuilder& jsonMode();
        RequestBuilder& temperature(float temp);
        RequestBuilder& topP(float p);
        RequestBuilder& topK(int k);
        RequestBuilder& maxTokens(int count);
        RequestBuilder& candidateCount(int count);
        RequestBuilder& stopSequences(const std::vector<std::string>& sequences);
        RequestBuilder& responseSchema(const nlohmann::json& schema);
        RequestBuilder& seed(int64_t seedValue);
        
        RequestBuilder& safety(HarmCategory category, HarmBlockThreshold threshold);

        RequestBuilder& tool(const Tool& tool);

        [[nodiscard]] GenerationResult generate() const;
        [[nodiscard]] GenerationResult stream(const StreamCallback& callback) const;

        [[nodiscard]] std::future<GenerationResult> generateAsync() const;
        [[nodiscard]] std::future<GenerationResult> streamAsync(const StreamCallback& callback) const;

    private:
        Client* client_;
        Model model_ = Model::GEMINI_2_5_FLASH;
        std::string systemInstruction_;
        GenerationConfig config_;
        std::vector<SafetySetting> safetySettings_;
        std::vector<Tool> tools_;
        std::vector<Part> parts_;
    };

}
#endif