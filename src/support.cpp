#include "gemini/support.h"

#include <algorithm>
#include <sstream>
#include <unordered_set>
#include <regex>

#include "gemini/utils.h"

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
    
    Base64String Base64String::fromBase64(const std::string& b64)
    {
        Base64String out;
        out.value = b64;
        return out;
    }
    
    Base64String::Base64String(const std::string& raw)
    {
        setFromRaw(raw);
    }

    Base64String& Base64String::operator=(const std::string& raw)
    {
        setFromRaw(raw);
        return *this;
    }

    std::string Base64String::str() const
    {
        return value;
    }

    Base64String::operator std::string() const
    {
        return value;
    }

    std::vector<unsigned char> Base64String::decode() const
    {
        return Utils::base64Decode(value);
    }

    std::string Base64String::decodeToString() const
    {
        auto bytes = Utils::base64Decode(value);
        return std::string{bytes.begin(), bytes.end()};
    }

    void Base64String::setFromRaw(const std::string& raw)
    {
        std::vector<unsigned char> bytes(raw.begin(), raw.end());
        value = Utils::base64Encode(bytes);
    }

    Timestamp::Timestamp(const std::string& s)
    {
        *this = fromString(s);
    }

    Timestamp& Timestamp::operator=(const std::string& s)
    {
        return *this = fromString(s);
    }

    Timestamp Timestamp::now(int digits_for_output)
    {
        using namespace std::chrono;
        auto now = system_clock::now();
        auto secs = duration_cast<seconds>(now.time_since_epoch());
        auto ns = duration_cast<nanoseconds>(now.time_since_epoch() - secs).count();
    
        int64_t epoch_seconds = secs.count();
        return fromEpochWithNanos(epoch_seconds, static_cast<int>(ns), digits_for_output);
    }

    Timestamp Timestamp::fromString(const std::string& s)
    {
        // regex: YYYY-MM-DDTHH:MM:SS[.fraction 1-9digits][Z|(+|-)HH:MM]
        static const std::regex re(R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(?:\.(\d{1,9}))?(Z|[+\-]\d{2}:\d{2})$)",
                                   std::regex::ECMAScript | std::regex::icase);
        std::smatch m;
        if (!std::regex_match(s, m, re))
            return {}; // invalid -> empty

        // capture groups:
        // [1]=year [2]=month [3]=day [4]=hour [5]=min [6]=sec [7]=fraction? [8]=offset(Z or ±HH:MM)
        int year  = std::stoi(m[1].str());
        int month = std::stoi(m[2].str());
        int day   = std::stoi(m[3].str());
        int hour  = std::stoi(m[4].str());
        int minute= std::stoi(m[5].str());
        int second= std::stoi(m[6].str());

        int nanos = 0;
        if (m[7].matched)
        {
            std::string frac = m[7].str();
            // pad to 9 digits (nanoseconds)
            if (frac.size() < 9) frac.append(9 - frac.size(), '0');
            else if (frac.size() > 9) frac = frac.substr(0, 9);
            nanos = std::stoi(frac);
        }

        std::string offset = m[8].str();
        // compute epoch seconds (UTC) using civil -> days -> seconds math, then apply offset
        auto epoch_seconds_opt = civilToEpochSeconds(year, month, day, hour, minute, second);
        if (!epoch_seconds_opt)
            return {}; // date out of range
        int64_t epoch_seconds = *epoch_seconds_opt;

        if (offset != "Z" && offset != "z")
            {
            // parse offset like +05:30 or -02:00 and subtract it to get UTC
            int sign = (offset[0] == '-') ? -1 : 1;
            int off_h = std::stoi(offset.substr(1,2));
            int off_m = std::stoi(offset.substr(4,2));
            int offset_seconds = sign * (off_h * 3600 + off_m * 60);
            // client's local time = epoch_seconds + offset_seconds -> to get UTC, subtract offset
            epoch_seconds -= offset_seconds;
        }

        // Format Z-normalized string using the smallest allowed fractional digits (0,3,6,9)
        std::string out = formatEpochZ(epoch_seconds, nanos);

        Timestamp t;
        t.value = out;
        return t;
    }

    Timestamp Timestamp::fromEpochWithNanos(int64_t epoch_seconds, int nanos, int digits_for_output)
    {
        // decide digits (0,3,6,9) based on digits_for_output request
        // valid digits_for_output: 0,3,6,9 otherwise normalize to nearest lower allowed
        int digits;
        if (digits_for_output == 0 || digits_for_output == 3 || digits_for_output == 6 || digits_for_output == 9)
            digits = digits_for_output;
        else if (digits_for_output < 3) digits = 0;
        else if (digits_for_output < 6) digits = 3;
        else if (digits_for_output < 9) digits = 6;
        else digits = 9;
    
        // If digits < 9 we should round/truncate nanos to that precision.
        int divisor = 1;
        for (int i = 0; i < 9 - digits; ++i) divisor *= 10;
        int out_nanos = (digits == 9) ? nanos : ((nanos / divisor) * divisor);
    
        Timestamp t;
        t.value = formatEpochWithDigits(epoch_seconds, out_nanos, digits);
        return t;
    }

    bool Timestamp::isValid() const
    {
        return !value.empty();
    }

    std::string Timestamp::str() const
    {
        return value;
    }

    Timestamp::operator std::string() const
    {
        return value;
    }

    std::optional<std::chrono::system_clock::time_point> Timestamp::to_time_point() const
    {
        if (!isValid())
            return std::nullopt;
        // parse the normalized string again (it is Z normalized)
        static const std::regex reZ(R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(?:\.(\d{1,9}))?Z$)",
                                   std::regex::ECMAScript | std::regex::icase);
        std::smatch m;
        if (!std::regex_match(value, m, reZ))
            return std::nullopt;
        int year  = std::stoi(m[1].str());
        int month = std::stoi(m[2].str());
        int day   = std::stoi(m[3].str());
        int hour  = std::stoi(m[4].str());
        int minute= std::stoi(m[5].str());
        int second= std::stoi(m[6].str());
        int nanos = 0;
        if (m[7].matched) {
            std::string frac = m[7].str();
            if (frac.size() < 9) frac.append(9 - frac.size(), '0');
            nanos = std::stoi(frac);
        }
        auto epoch_seconds_opt = civilToEpochSeconds(year, month, day, hour, minute, second);
        if (!epoch_seconds_opt)
            return std::nullopt;
        
        int64_t epoch_seconds = *epoch_seconds_opt;
        using namespace std::chrono;
        system_clock::duration d = duration_cast<system_clock::duration>(seconds(epoch_seconds) + nanoseconds(nanos));
        system_clock::time_point tp{ d };
        
        return tp;
    }

    int64_t Timestamp::days_from_civil(int64_t y, unsigned m, unsigned d) noexcept
    {
        y -= m <= 2;
        const int64_t era = (y >= 0 ? y : y-399) / 400;
        const unsigned yoe = static_cast<unsigned>(y - era * 400);      // [0, 399]
        const unsigned doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
        const unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;        // [0, 146096]
        return era * 146097 + static_cast<int64_t>(doe) - 719468;
    }

    std::optional<int64_t> Timestamp::civilToEpochSeconds(int year, int month, int day, int hour, int minute, int second) noexcept
    {
        // basic validation
        if (month < 1 || month > 12) return std::nullopt;
        if (day < 1 || day > 31) return std::nullopt;
        if (hour < 0 || hour > 23) return std::nullopt;
        if (minute < 0 || minute > 59) return std::nullopt;
        if (second < 0 || second > 60) return std::nullopt; // allow leap second 60
    
        int64_t days = days_from_civil(year, static_cast<unsigned>(month), static_cast<unsigned>(day));
        // seconds from days
        // watch overflow: days * 86400 must fit in int64
        int64_t sec_from_days = days * 86400LL;
        int64_t sec_of_day = hour * 3600 + minute * 60 + second;
        int64_t epoch_seconds = sec_from_days + sec_of_day;
        return epoch_seconds;
    }

    std::string Timestamp::formatEpochZ(int64_t epoch_seconds, int nanos)
    {
        // choose digits: smallest among {0,3,6,9} that fits nanos
        int digits;
        if (nanos == 0) digits = 0;
        else if ((nanos % 1000000) == 0) digits = 3;
        else if ((nanos % 1000) == 0) digits = 6;
        else digits = 9;
        return formatEpochWithDigits(epoch_seconds, nanos, digits);
    }

    std::string Timestamp::formatEpochWithDigits(int64_t epoch_seconds, int nanos, int digits)
    {
        // epoch_seconds -> y,m,d,h,min,sec using civil_from_days inverse
        int64_t days = floor_div(epoch_seconds, 86400);
        int64_t rem = epoch_seconds - days * 86400;
        if (rem < 0) { rem += 86400; --days; } // normalize
        int hour = static_cast<int>(rem / 3600);
        int minute = static_cast<int>((rem % 3600) / 60);
        int second = static_cast<int>(rem % 60);
    
        // civil_from_days to get year/month/day
        auto ymd = civil_from_days(days);
        int year = ymd[0], month = ymd[1], day = ymd[2];
    
        std::ostringstream oss;
        oss << std::setfill('0')
            << std::setw(4) << year << '-'
            << std::setw(2) << month << '-'
            << std::setw(2) << day << 'T'
            << std::setw(2) << hour << ':'
            << std::setw(2) << minute << ':'
            << std::setw(2) << second;
    
        if (digits > 0)
        {
            // adjust nanos to the digit count (truncate)
            int shrink = 9 - digits;
            int display = (shrink == 0) ? nanos : (nanos / static_cast<int>(std::pow(10, shrink)));
            oss << '.';
            oss << std::setw(digits) << std::setfill('0') << display;
        }
        oss << 'Z';
        return oss.str();
    }

    int64_t Timestamp::floor_div(int64_t x, int64_t y)
    {
        if (x >= 0)
            return x / y;
        return - ( (-x + y - 1) / y );
    }

    std::array<int, 3> Timestamp::civil_from_days(int64_t z) noexcept
    {
        z += 719468;
        const int64_t era = (z >= 0 ? z : z - 146096) / 146097;
        const unsigned doe = static_cast<unsigned>(z - era * 146097);          // [0, 146096]
        const unsigned yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
        const int year = static_cast<int>(yoe) + static_cast<int>(era) * 400;
        const unsigned doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
        const unsigned mp = (5*doy + 2) / 153;                                 // [0, 11]
        const unsigned day = doy - (153*mp+2)/5 + 1;                           // [1, 31]
        const unsigned month = mp + (mp < 10 ? 3 : -9);                         // [1, 12]
        const int y = year + (month <= 2);
        return {y, static_cast<int>(month), static_cast<int>(day)};
    }
}
