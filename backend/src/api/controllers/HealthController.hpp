#pragma once

/// @file HealthController.hpp
/// @brief Health check endpoint: GET /health

#include <drogon/HttpController.h>

namespace city_mapper {

class HealthController : public drogon::HttpController<HealthController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(HealthController::health, "/health", drogon::Get);
    METHOD_LIST_END

    /// @brief Returns service status and graph statistics.
    /// @details Response format:
    /// ```json
    /// {
    ///   "status": "ok",
    ///   "service": "city-mapper-api",
    ///   "version": "1.0.0",
    ///   "graph": {
    ///     "stations": 760,
    ///     "connections": 2428
    ///   },
    ///   "cache": {
    ///     "size": 42,
    ///     "capacity": 10000,
    ///     "hit_ratio": 0.85
    ///   }
    /// }
    /// ```
    void health(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};

}  // namespace city_mapper
