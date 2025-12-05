#pragma once

#ifndef GEMINI_REQUEST_BUILDER_H
#define GEMINI_REQUEST_BUILDER_H

#include <future>
#include <string>
#include <vector>

#include "generation_method.h"
#include "response.h"
#include "support.h"
#include "types/generating_content_api_types.h"

namespace GeminiCPP
{
    class Client;

    /**
     * @brief A helper class for constructing complex generation requests using a Fluent Interface.
     * * Allows method chaining to set various parameters like temperature, model, text content,
     * images, tools, and safety settings before executing the request.
     */
    class RequestBuilder
    {
    public:
        /**
         * @brief Constructs a new RequestBuilder.
         * * @param client Pointer to the Client instance that will execute the request.
         */
        explicit RequestBuilder(Client* client);

        /**
         * @brief Sets the model to be used for generation.
         * @param m The model to be used (e.g., gemini-2.0-flash).
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& model(const std::string& m);

        /**
         * @brief Appends text content to the user's message.
         * @param t The text string.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& text(const std::string& t);

        /**
         * @brief Appends an image to the user's message.
         * @param filepath Path to the image file.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& image(const std::string& filepath);

        /**
         * @brief Enables the Google Search grounding tool for this request.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& googleSearch();

        /**
         * @brief Enables the Code Execution tool for this request.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& codeExecution();

        /**
         * @brief Sets the context to a previously cached content resource.
         * @param cacheName Resource name of the cached content.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& cachedContent(const std::string& cacheName);

        /**
         * @brief Enables the Google Maps tool.
         * @param enableWidget Whether to request a rendered widget in the response.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& googleMaps(bool enableWidget = false);
        
        /**
         * @brief Adds a generic tool to the request.
         * @param tool The Tool object to add.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& tool(const Tool& tool);

        /**
         * @brief Sets the location context for tools that require it.
         * @param latitude Latitude coordinate.
         * @param longitude Longitude coordinate.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& location(double latitude, double longitude);

        /**
         * @brief Sets the tool configuration (e.g., function calling mode).
         * @param config The ToolConfig object.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& toolConfig(const ToolConfig& config);
        
        /**
         * @brief Sets the system instruction (system prompt).
         * @param instruction The instruction text.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& systemInstruction(const std::string& instruction);

        /**
         * @brief Forces the model to respond in valid JSON format.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& jsonMode();

        /**
         * @brief Sets the sampling temperature.
         * @param temp Value between 0.0 and 2.0. Higher values mean more random output.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& temperature(float temp);

        /**
         * @brief Sets the nucleus sampling probability (Top-P).
         * @param p Probability mass.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& topP(float p);

        /**
         * @brief Sets the Top-K sampling parameter.
         * @param k Number of top tokens to consider.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& topK(int k);

        /**
         * @brief Sets the maximum number of tokens to generate.
         * @param count Token count.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& maxTokens(int count);

        /**
         * @brief Sets the number of response candidates to generate.
         * @param count Candidate count.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& candidateCount(int count);

        /**
         * @brief Sets stop sequences where generation should halt.
         * @param sequences List of string sequences.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& stopSequences(const std::vector<std::string>& sequences);

        /**
         * @brief Defines a specific JSON schema for the response.
         * Automatically enables JSON mode.
         * @param schema The JSON schema definition.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& responseSchema(const nlohmann::json& schema);

        /**
         * @brief Sets a random seed for deterministic generation (if supported).
         * @param seedValue The integer seed.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& seed(int64_t seedValue);

        /**
         * @brief Configures the "Thinking" capability (Chain of Thought).
         * @param budget Token budget for thinking process.
         * @param includeThoughts Whether to include the thought trace in the response.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& thinking(int budget, bool includeThoughts = true);

        /**
         * @brief Configures "Thinking" using a predefined level.
         * @param level The thinking level.
         * @param includeThoughts Whether to include thoughts.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& thinking(ThinkingLevel level, bool includeThoughts = true);

        /**
         * @brief Enables "Thinking" with default settings.
         * @param includeThoughts Whether to include thoughts.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& thinking(bool includeThoughts = true);

        /**
         * @brief Sets the full ThinkingConfig object.
         * @param thinkingConfig Configuration object.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& thinking(const ThinkingConfig& thinkingConfig);
        
        /**
         * @brief Configures safety settings for a specific category.
         * @param category The harm category (e.g., HARM_CATEGORY_HATE_SPEECH).
         * @param threshold The block threshold.
         * @return RequestBuilder& Reference to self for chaining.
         */
        RequestBuilder& safety(HarmCategory category, HarmBlockThreshold threshold);
        
        /**
         * @brief Executes the request synchronously.
         * @return GenerationResult The API response.
         */
        [[nodiscard]] GenerationResult generate() const;

        /**
         * @brief Executes the request in streaming mode.
         * @param callback Function to handle incoming chunks.
         * @return GenerationResult Final result.
         */
        [[nodiscard]] GenerationResult stream(const StreamCallback& callback) const;

        /**
         * @brief Executes the request asynchronously.
         * @return std::future<GenerationResult> Future for the result.
         */
        [[nodiscard]] std::future<GenerationResult> generateAsync() const;

        /**
         * @brief Executes the request asynchronously in streaming mode.
         * @param callback Function to handle incoming chunks.
         * @return std::future<GenerationResult> Future for the final result.
         */
        [[nodiscard]] std::future<GenerationResult> streamAsync(const StreamCallback& callback) const;

    private:
        GenerationConfig& ensureConfig();
        Client* client_;
        std::string model_;
        Content currentContent_; 
        GenerateContentRequestBody requestPrototype_;
    };
}
#endif // GEMINI_REQUEST_BUILDER_H