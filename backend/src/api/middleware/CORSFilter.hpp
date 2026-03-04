#pragma once

/// @file CORSMiddleware.hpp
/// @brief CORS filter for cross-origin frontend access.

#include <drogon/HttpFilter.h>

namespace city_mapper {

/// @brief Adds CORS headers to all responses for frontend access.
/// @details Allows requests from any origin in development.
/// In production, restrict to the frontend domain.
class CORSFilter : public drogon::HttpFilter<CORSFilter> {
public:
    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&& fcb,
                  drogon::FilterChainCallback&& fccb) override;
};

}  // namespace city_mapper
