#include "gemini/response.h"
#include "gemini/utils.h"

#include <fstream>

namespace GeminiCPP
{
    std::string GenerationResult::text() const
    {
        std::string fullText;
        for (const auto& part : content.parts)
        {
            if (part.isText())
            {
                if (const auto* txtData = part.getText())
                    fullText += txtData->text;
            }
        }
        return fullText;
    }

    bool GenerationResult::saveFile(const std::string& filepath, size_t blobIndex) const
    {
        size_t currentBlob = 0;
        for (const auto& part : content.parts)
        {
            if (part.isBlob())
            {
                if (currentBlob == blobIndex)
                {
                    if (const auto* blob = part.getBlob())
                    {
                        auto data = Utils::base64Decode(blob->data);
                        std::ofstream file(filepath, std::ios::binary);
                        if (file.is_open())
                        {
                            file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
                            return true;
                        }
                    }

                    return false;
                }
                currentBlob++;
            }
        }
        return false;
    }

    int GenerationResult::saveAllFiles(const std::string& directory, const std::string& prefix) const
    {
        namespace fs = std::filesystem;
            
        if (!fs::exists(directory))
        {
            fs::create_directories(directory);
        }

        int savedCount = 0;
        size_t blobIndex = 0;

        for (const auto& part : content.parts)
        {
            if (part.isBlob())
            {
                if (const auto* blob = part.getBlob())
                {
                    std::string ext = Utils::mimeToExtension(blob->mimeType);
                    
                    fs::path fullPath = fs::path(directory) / (prefix + "_" + std::to_string(blobIndex) + ext);

                    if (saveFile(fullPath.string(), blobIndex))
                    {
                        savedCount++;
                    }
                    blobIndex++;
                }

            }
        }
        return savedCount;
    }
    
    GenerationResult GenerationResult::Success(Content c, int code, int inTok,
        int outTok, FinishReason reason, std::optional<GroundingMetadata> metadata)
    {
        GenerationResult r;
        r.success = true;
        r.content = std::move(c);
        r.statusCode = static_cast<HttpStatusCode>(code);
        r.inputTokens = inTok;
        r.outputTokens = outTok;
        r.totalTokens = inTok + outTok;
        r.finishReason = reason;
        r.groundingMetadata = std::move(metadata);
        return r;
    }

    GenerationResult GenerationResult::Failure(std::string err, int code, FinishReason reason)
    {
        GenerationResult r;
        r.success = false;
        r.errorMessage = std::move(err);
        r.statusCode = static_cast<HttpStatusCode>(code);
        r.finishReason = reason;
        return r;
    }

    std::optional<nlohmann::json> GenerationResult::asJson() const
    {
        std::string txt = text();
        if (txt.empty()) return std::nullopt;

        try 
        {
            return nlohmann::json::parse(txt);
        } 
        catch (...) 
        {
            size_t startObj = txt.find('{');
            size_t startArr = txt.find('[');
                
            size_t start = std::string::npos;

            if (startObj != std::string::npos && startArr != std::string::npos)
                start = std::min(startObj, startArr);
            else if (startObj != std::string::npos)
                start = startObj;
            else if (startArr != std::string::npos)
                start = startArr;

            size_t endObj = txt.rfind('}');
            size_t endArr = txt.rfind(']');
                
            size_t end = std::string::npos;

            if (endObj != std::string::npos && endArr != std::string::npos)
                end = std::max(endObj, endArr);
            else if (endObj != std::string::npos)
                end = endObj;
            else if (endArr != std::string::npos)
                end = endArr;

            if (start != std::string::npos && end != std::string::npos && end > start) 
            {
                try
                {
                    std::string cleanJson = txt.substr(start, end - start + 1);
                    return nlohmann::json::parse(cleanJson);
                } catch(...)
                {
                    return std::nullopt;
                }
            }
                
            return std::nullopt;
        }
    }

}
