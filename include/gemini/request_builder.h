#pragma once

#ifndef GEMINI_REQUEST_BUILDER_H
#define GEMINI_REQUEST_BUILDER_H

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

        GenerationResult generate() const;

    private:
        Client* client_;
        Model model_ = Model::GEMINI_2_5_FLASH;
        std::string systemInstruction_;
        std::vector<Part> parts_;
    };

}
#endif