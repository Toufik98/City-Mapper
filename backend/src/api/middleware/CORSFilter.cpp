/// @file CORSFilter.cpp
/// @brief CORS filter implementation.

#include "CORSFilter.hpp"

namespace city_mapper {

void CORSFilter::doFilter(const drogon::HttpRequestPtr& req,
                           drogon::FilterCallback&& fcb,
                           drogon::FilterChainCallback&& fccb) {
    // Handle preflight OPTIONS requests
    if (req->method() == drogon::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers",
                        "Content-Type, Authorization, Accept");
        resp->addHeader("Access-Control-Max-Age", "86400");
        resp->setStatusCode(drogon::k204NoContent);
        fcb(resp);
        return;
    }

    // Continue the filter chain — CORS headers added via Drogon's
    // AOP (Aspect Oriented Programming) in main.cpp
    fccb();
}

}  // namespace city_mapper
