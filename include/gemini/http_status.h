#pragma once

#ifndef GEMINI_HTTP_STATUS_H
#define GEMINI_HTTP_STATUS_H

#include <string_view>
#include "frenum.h"

namespace GeminiCPP
{
    FrenumClassInNamespace(GeminiCPP, HttpStatusCode, int,
        CONTINUE = 100,
        SWITCHING_PROTOCOLS = 101,
        // 102 -> PROCESSING -> deprecated
        EARLY_HINTS = 103, 
    
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NON_AUTHORITATIVE_INFORMATION = 203,
        NO_CONTENT = 204,
        RESET_CONTENT = 205,
        PARTIAL_CONTENT = 206,
        MULTI_STATUS = 207,
        ALREADY_REPORTED = 208,
        IM_USED = 226,

        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        FOUND = 302,
        SEE_OTHER = 303,
        NOT_MODIFIED = 304,
        // 305 -> USE_PROXY -> deprecated
        // 306 -> unused but reserved
        TEMPORARY_REDIRECT = 307,
        PERMANENT_REDIRECT = 308,

        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        PAYMENT_REQUIRED = 402,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        NOT_ACCEPTABLE = 406,
        PROXY_AUTHENTICATION_REQUIRED = 407,
        TIMEOUT = 408,
        CONFLICT = 409,
        GONE = 410,
        LENGTH_REQUIRED = 411,
        PRECONDITION_FAILED = 412,
        PAYLOAD_TOO_LARGE = 413,
        URI_TOO_LONG = 414,
        UNSUPPORTED_MEDIA_TYPE = 415,
        RANGE_NOT_SATISFIABLE = 416,
        EXPECTATION_FAILED = 417,
        // 418 -> I'm a teapot
        MISDIRECT_REQUEST = 421,
        UNPROCESSABLE_CONTENT = 422,
        LOCKED = 423,
        FAILED_DEPENDENCY = 424,
        TOO_EARLY = 425, // Experimental
        UPGRADE_REQUIRED = 426,
        PRECONDITION_REQUIRED = 428,
        TOO_MANY_REQUESTS = 429,
        REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
        UNAVAILABLE_FOR_LEGAL_REASONS = 451,

        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503,
        GATEWAY_TIMEOUT = 504,
        HTTP_VERSION_NOT_SUPPORTED = 505,
        VARIANT_ALSO_NEGOTIATES = 506,
        INSUFFICIENT_STORAGE = 507,
        LOOP_DETECTED = 508,
        NOT_EXTENDED = 509,
        NETWORK_AUTHENTICATION_REQUIRED = 511,
        
        UNKNOWN = 0
    );

    class HttpStatusHelper
    {
    public:
        [[nodiscard]] static constexpr bool isSuccess(HttpStatusCode code)
        {
            return frenum::value(code) >= 200 && frenum::value(code) < 300;
        }

        [[nodiscard]] static constexpr bool isSuccess(int code)
        {
            return code >= 200 && code < 300;
        }

        [[nodiscard]] static constexpr std::string_view toString(HttpStatusCode code)
        {
            return frenum::to_string_view<HttpStatusCode>(code);
        }
    };
}

#endif // GEMINI_HTTP_STATUS_H