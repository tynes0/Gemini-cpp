#include "gemini/support.h"

#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace GeminiCPP::Support
{
    namespace 
    {
        const std::unordered_set<std::string>& supportedRatios()
        {
            static const std::unordered_set<std::string> s = {
                "1:1", "2:3", "3:2", "3:4", "4:3", "9:16", "16:9", "21:9"
            };
            return s;
        }

        const std::unordered_map<std::string, ImageSize::Value>& supportedImageSizeStrings()
        {
            static const std::unordered_map<std::string, ImageSize::Value> m = {
                {"1K", ImageSize::Value::K1},
                {"2K", ImageSize::Value::K2},
                {"4K", ImageSize::Value::K4}
            };
            return m;
        }

        const std::unordered_map<int, ImageSize::Value>& supportedImageSizeInts()
        {
            static const std::unordered_map<int, ImageSize::Value> m = {
                {1, ImageSize::Value::K1},
                {2, ImageSize::Value::K2},
                {4, ImageSize::Value::K4}
            };
            return m;
        }

        // Canonical list (exact API strings)
        constexpr std::array<const char*, 30> LC_Canonical = {
            "de-DE", "en-AU", "en-GB", "en-IN", "en-US",
            "es-US", "fr-FR", "hi-IN", "pt-BR", "ar-XA",
            "es-ES", "fr-CA", "id-ID", "it-IT", "ja-JP",
            "tr-TR", "vi-VN", "bn-IN", "gu-IN", "kn-IN",
            "ml-IN", "mr-IN", "ta-IN", "te-IN", "nl-NL",
            "ko-KR", "cmn-CN", "pl-PL", "ru-RU", "th-TH"
        };

        // Lowercase equivalents for lookup
        constexpr std::array<const char*, LC_Canonical.size()> LC_Lowercase = {
            "de-de", "en-au", "en-gb", "en-in", "en-us",
            "es-us", "fr-fr", "hi-in", "pt-br", "ar-xa",
            "es-es", "fr-ca", "id-id", "it-it", "ja-jp",
            "tr-tr", "vi-vn", "bn-in", "gu-in", "kn-in",
            "ml-in", "mr-in", "ta-in", "te-in", "nl-nl",
            "ko-kr", "cmn-cn", "pl-pl", "ru-ru", "th-th"
        };

        std::string toLower(const std::string& str)
        {
            std::string s = str;
            std::transform(s.begin(), s.end(), s.begin(),
               [](unsigned char c){ return std::tolower(c); });
            return s;
        }
    }

    AspectRatio::AspectRatio(int w_, int h_)
    {
        if (isSupported(w_, h_))
        {
            w = w_;
            h = h_;
        }
        else
        {
            *this = AspectRatio();
        }
    }

    AspectRatio::AspectRatio(const std::string& s)
    {
        if (!parseString(s, w, h) || !isSupported(w, h))
        {
            *this = AspectRatio();
        }
    }

    AspectRatio& AspectRatio::operator=(const std::string& s)
    {
        int tw, th;
        if (!parseString(s, tw, th) || !isSupported(tw, th))
        {
            *this = AspectRatio();
        }
        else
        {
            w = tw;
            h = th;
        }
        return *this;
    }

    std::string AspectRatio::str() const
    {
        if (isEmpty())
            return "";
        
        return std::to_string(w) + ":" + std::to_string(h);
    }

    AspectRatio::operator std::string() const
    {
        return str();
    }

    bool AspectRatio::isEmpty() const
    {
        return w == 0 || h == 0;
    }

    bool AspectRatio::operator==(const AspectRatio& other) const
    {
        return w == other.w && h == other.h;
    }

    bool AspectRatio::parseString(const std::string& s, int& outW, int& outH)
    {
        if (s.empty()) return false;

        char colon;
        std::istringstream iss(s);
        if (!(iss >> outW >> colon >> outH)) return false;
        if (colon != ':') return false;
        return true;
    }

    bool AspectRatio::isSupported(int w, int h)
    {
        if (w <= 0 || h <= 0)
            return false;
        std::string key = std::to_string(w) + ":" + std::to_string(h);
        return supportedRatios().contains(key);
    }

    ImageSize::ImageSize(Value v)
        : value(v) 
    {
    }

    ImageSize::ImageSize(const std::string& s)
    {
        auto it = supportedImageSizeStrings().find(s);
        value = (it != supportedImageSizeStrings().end()) ? it->second : Value::K1;
    }

    ImageSize::ImageSize(int k)
    {
        auto it = supportedImageSizeInts().find(k);
        value = (it != supportedImageSizeInts().end()) ? it->second : Value::K1;
    }

    ImageSize& ImageSize::operator=(Value v)
    {
        value = v;
        return *this;
    }

    ImageSize& ImageSize::operator=(const std::string& s)
    {
        auto it = supportedImageSizeStrings().find(s);
        value = (it != supportedImageSizeStrings().end()) ? it->second : Value::K1;
        return *this;
    }

    ImageSize& ImageSize::operator=(int k)
    {
        auto it = supportedImageSizeInts().find(k);
        value = (it != supportedImageSizeInts().end()) ? it->second : Value::K1;
        return *this;
    }

    std::string ImageSize::str() const
    {
        switch (value)
        {
        case Value::K1: return "1K";
        case Value::K2: return "2K";
        case Value::K4: return "4K";
        }
        return "1K";
    }

    ImageSize::operator std::string() const
    {
        return str();
    }
    

    ImageSize::operator int() const
    {
        return static_cast<int>(value);
    }

    bool ImageSize::operator==(const ImageSize& other) const
    {
        return value == other.value;
    }

    LanguageCode::LanguageCode(const std::string& v)
    {
        set(v);
    }

    LanguageCode& LanguageCode::operator=(const std::string& v)
    {
        set(v);
        return *this;
    }

    std::string LanguageCode::str() const
    {
        return value;
    }

    LanguageCode::operator std::string() const
    {
        return value;
    }

    bool LanguageCode::operator==(const LanguageCode& other) const
    {
        return value == other.value;
    }

    void LanguageCode::set(const std::string& v)
    {
        const auto low = toLower(v);

        for (size_t i = 0; i < LC_Lowercase.size(); ++i)
        {
            if (low == LC_Lowercase[i])
            {
                value = LC_Canonical[i];
                return;
            }
        }

        value = Default;
    }
}
