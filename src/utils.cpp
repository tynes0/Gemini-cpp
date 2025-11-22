#include "gemini/utils.h"

namespace GeminiCPP
{
    std::string Utils::fileToBase64(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
        {
            GEMINI_ERROR("The file could not be read: {}", filepath);
            return "";
        }
        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});
        return base64Encode(buffer);
    }

    std::string Utils::getMimeType(const std::string& filepath)
    {
        namespace fs = std::filesystem;
        auto ext = fs::path(filepath).extension().string();
        
        if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
        if (ext == ".png") return "image/png";
        if (ext == ".webp") return "image/webp";
        if (ext == ".pdf") return "application/pdf";
        
        return "image/jpeg"; // Default
    }

    std::vector<unsigned char> Utils::base64Decode(const std::string& encoded_string)
    {
        static const std::string base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        auto is_base64 = [&](unsigned char c)
        {
            return (isalnum(c) || (c == '+') || (c == '/'));
        };

        int in_len = static_cast<int>(encoded_string.size());
        int i = 0, j = 0, in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::vector<unsigned char> ret;

        while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_]; in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++)
                    char_array_4[i] = static_cast<unsigned char>(base64_chars.find(static_cast<char>(char_array_4[i])));

                char_array_3[0] = static_cast<unsigned char>((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
                char_array_3[1] = static_cast<unsigned char>(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
                char_array_3[2] = static_cast<unsigned char>(((char_array_4[2] & 0x3) << 6) + char_array_4[3]);

                for (i = 0; (i < 3); i++)
                    ret.push_back(char_array_3[i]);
                
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 4; j++)
                char_array_4[j] = 0;
            
            for (j = 0; j < 4; j++)
                char_array_4[j] = static_cast<unsigned char>(base64_chars.find(static_cast<char>(char_array_4[j])));

            char_array_3[0] = static_cast<unsigned char>((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
            char_array_3[1] = static_cast<unsigned char>(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
            char_array_3[2] = static_cast<unsigned char>(((char_array_4[2] & 0x3) << 6) + char_array_4[3]);

            for (j = 0; (j < i - 1); j++)
                ret.push_back(char_array_3[j]);
        }

        return ret;
    }

    // Standard base64 encode algorithm
    std::string Utils::base64Encode(const std::vector<unsigned char>& data)
    {
        static constexpr const char* base64_chars = 
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        
        std::string ret;
        int i = 0, j = 0;
        unsigned char char_array_3[3], char_array_4[4];

        for (const unsigned char c : data)
        {
            char_array_3[i++] = c;
            if (i == 3)
            {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = static_cast<unsigned char>(((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4));
                char_array_4[2] = static_cast<unsigned char>(((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6));
                char_array_4[3] = char_array_3[2] & 0x3f;
                
                for(i = 0; i <4 ; i++)
                    ret += base64_chars[char_array_4[i]];
                
                i = 0;
            }
        }
        if (i)
        {
            for(j = i; j < 3; j++) char_array_3[j] = '\0';
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = static_cast<unsigned char>(((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4));
            char_array_4[2] = static_cast<unsigned char>(((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6));
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for (j = 0; j < i + 1; j++)
                ret += base64_chars[char_array_4[j]];
            
            while((i++ < 3)) ret += '=';
        }
        return ret;
    }
}

