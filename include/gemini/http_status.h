#pragma once

#ifndef GEMINI_HTTP_STATUS_H
#define GEMINI_HTTP_STATUS_H

#include <string_view>
#include <frenum.h>

namespace GeminiCPP
{
    FrenumClassInNamespace(GeminiCPP, HttpStatusCode, int,
        OK = 200,
        CREATED = 201,
        NO_CONTENT = 204,

        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        TIMEOUT = 408,
        PAYLOAD_TOO_LARGE = 413,
        TOO_MANY_REQUESTS = 429,

        INTERNAL_SERVER_ERROR = 500,
        SERVICE_UNAVAILABLE = 503,
        
        UNKNOWN = 0
    );

    class HttpStatusHelper
    {
    public:
        static constexpr bool isSuccess(HttpStatusCode code)
        {
            return frenum::value(code) >= 200 && frenum::value(code) < 300;
        }

        static constexpr bool isSuccess(int code)
        {
            return code >= 200 && code < 300;
        }

        static constexpr std::string_view toString(HttpStatusCode code)
        {
            return frenum::to_string_view<HttpStatusCode>(code);
        }
    };
}

#endif // GEMINI_HTTP_STATUS_H