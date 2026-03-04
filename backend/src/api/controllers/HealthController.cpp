/// @file HealthController.cpp
/// @brief Implementation of the health check endpoint.

#include "HealthController.hpp"

#include <json/json.h>

#include "../../core/graph/Graph.hpp"
#include "../../core/cache/RouteCache.hpp"

namespace city_mapper {

// Declared in main.cpp — the global application state
extern Graph* g_graph;
extern RouteCache* g_route_cache;

void HealthController::health(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {

    Json::Value json;
    json["status"] = "ok";
    json["service"] = "city-mapper-api";
    json["version"] = "1.0.0";

    if (g_graph) {
        Json::Value graph_info;
        graph_info["stations"] = static_cast<Json::UInt64>(g_graph->station_count());
        graph_info["connections"] = static_cast<Json::UInt64>(g_graph->connection_count());
        json["graph"] = graph_info;
    }

    if (g_route_cache) {
        Json::Value cache_info;
        cache_info["size"] = static_cast<Json::UInt64>(g_route_cache->size());
        cache_info["capacity"] = static_cast<Json::UInt64>(g_route_cache->capacity());
        cache_info["hit_ratio"] = g_route_cache->hit_ratio();
        cache_info["hits"] = static_cast<Json::UInt64>(g_route_cache->hit_count());
        cache_info["misses"] = static_cast<Json::UInt64>(g_route_cache->miss_count());
        json["cache"] = cache_info;
    }

    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    callback(resp);
}

}  // namespace city_mapper
