#include "gemini/duration.h"

#include "gemini/logger.h"
#include "nlohmann/json.hpp"

namespace GeminiCPP
{
    Duration Duration::fromSeconds(int64_t s)
    {
        return {
            .seconds = s,
            .nanos = 0
        };
    }

    Duration Duration::fromMillis(int64_t ms)
    { 
        return {
            .seconds = ms / 1000,
            .nanos = static_cast<int32_t>((ms % 1000) * 1000000)
        }; 
    }

    Duration Duration::fromMinutes(int64_t m)
    {
        return {
            .seconds = m * 60,
            .nanos = 0
        };
    }

    Duration Duration::fromString(const std::string& str)
    {
        Duration d;
        std::string s = str;
        
        if (s.empty())
            return d;
        
        if (s.back() == 's')
            s.pop_back();

        size_t decimalPos = s.find('.');
        if (decimalPos == std::string::npos)
        {
            try
            {
                d.seconds = std::stoll(s);
            }
            catch(const std::exception& e)
            {
                GEMINI_ERROR("String to long long conversation failed: {}", e.what());
            }
        }
        else
        {
            std::string secPart = s.substr(0, decimalPos);
            std::string nanoPart = s.substr(decimalPos + 1);

            try
            {
                d.seconds = std::stoll(secPart);
            }
            catch(const std::exception& e)
            {
                GEMINI_ERROR("String to long long conversation failed: {}", e.what());
            }

            if (nanoPart.length() > 9)
                nanoPart = nanoPart.substr(0, 9);
            else
                while (nanoPart.length() < 9)
                    nanoPart += '0';

            try
            {
                d.nanos = std::stoi(nanoPart);
            }
            catch(const std::exception& e)
            {
                GEMINI_ERROR("String to int conversation failed: {}", e.what());
            }
        }
        return d;
    }

    Duration Duration::fromJson(const nlohmann::json& j)
    {
        if (j.is_string())
            return fromString(j.get<std::string>());

        return { .seconds = 0, .nanos = 0 };
    }

    nlohmann::json Duration::toJson() const
    {
        return toString();
    }
    
    std::string Duration::toString() const
    {
        std::ostringstream oss;
        oss << seconds;
        
        if (nanos != 0)
        {
            oss << ".";
            // Protobuf standard: 3, 6, or 9 digits
            // For simplicity, we can print the required precision without truncating zeros.
            // However, the API is generally flexible. Let's produce a clean output.
            
            std::string nStr = std::to_string(std::abs(nanos));
            // Add leading zeros to round up to 9 digits (e.g., not 50 -> 000000050, but 500ms -> .500)
            // Correction: If nanos is 500,000,000, this means .5.
            // If nanos is 1, this means .000000001.
            
            oss << std::setw(9) << std::setfill('0') << std::abs(nanos);
            
            std::string s = oss.str();
            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
            
            if (s.back() == '.')
                s.pop_back();
            
            return s + "s";
        }
        
        oss << "s";
        return oss.str();
    }
}
