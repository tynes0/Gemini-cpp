#include "gemini/generation_method.h"

namespace GeminiCPP
{
    GenerationMethod operator|(GenerationMethod a, GenerationMethod b)
    {
        return static_cast<GenerationMethod>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    GenerationMethod& operator|=(GenerationMethod& a, GenerationMethod b)
    {
        a = a | b;
        return a;
    }

    GenerationMethod GenerationMethodHelper::fromString(const std::string& method)
    {
        if (method == "asyncBatchEmbedContent") return GM_ASYNC_BATCH_EMBED_CONTENT;
        if (method == "batchEmbedContents")     return GM_BATCH_EMBED_CONTENTS;
        if (method == "batchEmbedText")         return GM_BATCH_EMBED_TEXT;
        if (method == "batchGenerateContent")   return GM_BATCH_GENERATE_CONTENT;
        if (method == "countMessageTokens")     return GM_COUNT_MESSAGE_TOKENS;
        if (method == "countTextTokens")        return GM_COUNT_TEXT_TOKENS;
        if (method == "countTokens")            return GM_COUNT_TOKENS;
        if (method == "embedContent")           return GM_EMBED_CONTENT;
        if (method == "embedText")              return GM_EMBED_TEXT;
        if (method == "generateContent")        return GM_GENERATE_CONTENT;
        if (method == "generateMessage")        return GM_GENERATE_MESSAGE;
        if (method == "generateText")           return GM_GENERATE_TEXT;
        if (method == "get")                    return GM_GET;
        if (method == "list")                   return GM_LIST;
        if (method == "predict")                return GM_PREDICT;
        if (method == "predictLongRunning")     return GM_PREDICT_LONG_RUNNING;
        if (method == "streamGenerateContent")  return GM_STREAM_GENERATE_CONTENT;
        return GM_UNSPECIFIED;
    }

    std::string GenerationMethodHelper::toString(GenerationMethod method)
    {
        switch (method)
        {
        case GM_ASYNC_BATCH_EMBED_CONTENT:  return "asyncBatchEmbedContents";
        case GM_BATCH_EMBED_CONTENTS:       return "batchEmbedContents";
        case GM_BATCH_EMBED_TEXT:           return "batchEmbedText";
        case GM_BATCH_GENERATE_CONTENT:     return "batchGenerateContent";
        case GM_COUNT_MESSAGE_TOKENS:       return "countMessageTokens";
        case GM_COUNT_TEXT_TOKENS:          return "countTextTokens";
        case GM_COUNT_TOKENS:               return "countTokens";
        case GM_EMBED_CONTENT:              return "embedContent";
        case GM_EMBED_TEXT:                 return "embedText";
        case GM_GENERATE_CONTENT:           return "generateContent";
        case GM_GENERATE_MESSAGE:           return "generateMessage";
        case GM_GENERATE_TEXT:              return "generateText";
        case GM_GET:                        return "get";
        case GM_LIST:                       return "list";
        case GM_PREDICT:                    return "predict";
        case GM_PREDICT_LONG_RUNNING:       return "predictLongRunning";
        case GM_STREAM_GENERATE_CONTENT:    return "streamGenerateContent";
        case GM_NONE:
        case GM_UNSPECIFIED:
            break;
        }
        return "";    
    }

    std::string GenerationMethodHelper::bitmaskToString(uint32_t flags)
    {
        std::string s;
        if (flags & GM_ASYNC_BATCH_EMBED_CONTENT)   s += "asyncBatchEmbedContents, ";
        if (flags & GM_BATCH_EMBED_CONTENTS)        s += "batchEmbedContents, ";
        if (flags & GM_BATCH_EMBED_TEXT)            s += "batchEmbedText, ";
        if (flags & GM_BATCH_GENERATE_CONTENT)      s += "batchGenerateContent, ";
        if (flags & GM_COUNT_MESSAGE_TOKENS)        s += "countMessageTokens, ";
        if (flags & GM_COUNT_TEXT_TOKENS)           s += "countTextTokens, ";
        if (flags & GM_COUNT_TOKENS)                s += "countTokens, ";
        if (flags & GM_EMBED_CONTENT)               s += "embedContent, ";
        if (flags & GM_EMBED_TEXT)                  s += "embedText, ";
        if (flags & GM_GENERATE_CONTENT)            s += "generateContent, ";
        if (flags & GM_GENERATE_MESSAGE)            s += "generateMessage, ";
        if (flags & GM_GENERATE_TEXT)               s += "generateText, ";
        if (flags & GM_GET)                         s += "get, ";
        if (flags & GM_LIST)                        s += "list, ";
        if (flags & GM_PREDICT)                     s += "predict, ";
        if (flags & GM_PREDICT_LONG_RUNNING)        s += "predictLongRunning, ";
        if (flags & GM_STREAM_GENERATE_CONTENT)     s += "streamGenerateContent, ";
        
        if (s.length() > 2)
            s.resize(s.length() - 2);
        
        return "[" + s + "]";
    }
}
