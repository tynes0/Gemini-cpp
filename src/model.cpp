#include "gemini/model.h"

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

    std::vector<std::string> GenerationMethodHelper::bitmaskToStringArray(uint32_t flags)
    {
        std::vector<std::string> result;
        if (flags & GM_ASYNC_BATCH_EMBED_CONTENT)   result.emplace_back("asyncBatchEmbedContents");
        if (flags & GM_BATCH_EMBED_CONTENTS)        result.emplace_back("batchEmbedContents");
        if (flags & GM_BATCH_EMBED_TEXT)            result.emplace_back("batchEmbedText");
        if (flags & GM_BATCH_GENERATE_CONTENT)      result.emplace_back("batchGenerateContent");
        if (flags & GM_COUNT_MESSAGE_TOKENS)        result.emplace_back("countMessageTokens");
        if (flags & GM_COUNT_TEXT_TOKENS)           result.emplace_back("countTextTokens");
        if (flags & GM_COUNT_TOKENS)                result.emplace_back("countTokens");
        if (flags & GM_EMBED_CONTENT)               result.emplace_back("embedContent");
        if (flags & GM_EMBED_TEXT)                  result.emplace_back("embedText");
        if (flags & GM_GENERATE_CONTENT)            result.emplace_back("generateContent");
        if (flags & GM_GENERATE_MESSAGE)            result.emplace_back("generateMessage");
        if (flags & GM_GENERATE_TEXT)               result.emplace_back("generateText");
        if (flags & GM_GET)                         result.emplace_back("get");
        if (flags & GM_LIST)                        result.emplace_back("list");
        if (flags & GM_PREDICT)                     result.emplace_back("predict");
        if (flags & GM_PREDICT_LONG_RUNNING)        result.emplace_back("predictLongRunning");
        if (flags & GM_STREAM_GENERATE_CONTENT)     result.emplace_back("streamGenerateContent");

        return result;
    }

    std::string_view ModelHelper::stringRepresentation(Model model)
    {
        switch (model)
        {
        case Model::GEMINI_2_0_FLASH:               return "gemini-2.0-flash";
        case Model::GEMINI_2_0_FLASH_LITE:          return "gemini-2.0-flash-lite";
        case Model::GEMINI_2_5_FLASH:               return "gemini-2.5-flash";
        case Model::GEMINI_2_5_FLASH_IMAGE:         return "gemini-2.5-flash-image";
        case Model::GEMINI_2_5_FLASH_TTS:           return "gemini-2.5-flash-preview-tts";
        case Model::GEMINI_2_5_FLASH_LITE:          return "gemini-2.5-flash-lite";
        case Model::GEMINI_2_5_PRO:                 return "gemini-2.5-pro";
        case Model::GEMINI_2_5_PRO_TTS:             return "gemini-2.5-pro-preview-tts";
        case Model::GEMINI_3_0_PRO_PREVIEW:         return "gemini-3-pro-preview";
        case Model::GEMINI_3_0_PRO_IMAGE_PREVIEW:   return "gemini-3-pro-image-preview";
        case Model::GEMINI_FLASH_LATEST:            return "gemini-flash-latest";
        case Model::GEMINI_PRO_LATEST:              return "gemini-pro-latest";
        }
        return "gemini-2.5-flash";
    }

    Model ModelHelper::fromStringRepresentation(const std::string& str)
    {
        if (str == "gemini-2.0-flash") return Model::GEMINI_2_0_FLASH;
        if (str == "gemini-2.0-flash-lite") return Model::GEMINI_2_0_FLASH_LITE;
        if (str == "gemini-2.5-flash") return Model::GEMINI_2_5_FLASH;
        if (str == "gemini-2.5-flash-image") return Model::GEMINI_2_5_FLASH_IMAGE;
        if (str == "gemini-2.5-flash-preview-tts") return Model::GEMINI_2_5_FLASH_TTS;
        if (str == "gemini-2.5-flash-lite") return Model::GEMINI_2_5_FLASH_LITE;
        if (str == "gemini-2.5-pro") return Model::GEMINI_2_5_PRO;
        if (str == "gemini-2.5-pro-preview-tts") return Model::GEMINI_2_5_PRO_TTS;
        if (str == "gemini-3-pro-preview") return Model::GEMINI_3_0_PRO_PREVIEW;
        if (str == "gemini-3-pro-image-preview") return Model::GEMINI_3_0_PRO_IMAGE_PREVIEW;
        if (str == "gemini-flash-latest") return Model::GEMINI_FLASH_LATEST;
        if (str == "gemini-pro-latest") return Model::GEMINI_PRO_LATEST;

        return Model::GEMINI_2_5_FLASH;
    }
}
