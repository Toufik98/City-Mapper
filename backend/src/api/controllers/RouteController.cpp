/// @file RouteController.cpp
/// @brief Implementation of route computation REST endpoint.

#include "RouteController.hpp"

#include <chrono>
#include <json/json.h>
#include <memory>

#include "../../core/algorithms/AStar.hpp"
#include "../../core/algorithms/Dijkstra.hpp"
#include "../../core/algorithms/IPathfinder.hpp"
#include "../../core/cache/RouteCache.hpp"
#include "../../core/graph/Graph.hpp"
#include "../../core/graph/RouteResult.hpp"
#include "../../core/utils/TimeFormatter.hpp"

namespace city_mapper {

extern Graph* g_graph;
extern RouteCache* g_route_cache;

/// @brief Serializes a RouteResult to JSON.
static Json::Value route_result_to_json(const RouteResult& result,
                                        double computation_ms) {
    Json::Value json;
    json["found"] = result.found;
    json["algorithm"] = result.algorithm;

    if (!result.found) {
        json["error"] = "No route found between the specified stations";
        return json;
    }

    json["total_duration_seconds"] = static_cast<Json::UInt64>(result.total_duration_seconds);
    json["total_duration_display"] = format_duration(result.total_duration_seconds);
    json["total_duration_compact"] = format_duration_compact(result.total_duration_seconds);
    json["total_transfers"] = result.total_transfers;
    json["total_stations"] = result.total_stations;
    json["computation_time_ms"] = computation_ms;

    Json::Value segments_json(Json::arrayValue);
    for (const auto& segment : result.segments) {
        Json::Value seg;
        seg["from_station"]["id"] = static_cast<Json::UInt64>(segment.from_station.id);
        seg["from_station"]["name"] = segment.from_station.name;
        seg["from_station"]["line_id"] = segment.from_station.line_id;
        seg["from_station"]["latitude"] = segment.from_station.latitude;
        seg["from_station"]["longitude"] = segment.from_station.longitude;

        seg["to_station"]["id"] = static_cast<Json::UInt64>(segment.to_station.id);
        seg["to_station"]["name"] = segment.to_station.name;
        seg["to_station"]["line_id"] = segment.to_station.line_id;
        seg["to_station"]["latitude"] = segment.to_station.latitude;
        seg["to_station"]["longitude"] = segment.to_station.longitude;

        seg["duration_seconds"] = static_cast<Json::UInt64>(segment.duration_seconds);
        seg["duration_display"] = format_duration(segment.duration_seconds);
        seg["type"] = connection_type_to_string(segment.type);
        seg["line_id"] = segment.line_id;
        seg["line_name"] = segment.line_name;

        segments_json.append(seg);
    }
    json["segments"] = segments_json;

    // Raw path for debugging / advanced clients
    Json::Value raw_path_json(Json::arrayValue);
    for (const auto& [id, cost] : result.raw_path) {
        Json::Value entry;
        entry["station_id"] = static_cast<Json::UInt64>(id);
        entry["cumulative_seconds"] = static_cast<Json::UInt64>(cost);
        raw_path_json.append(entry);
    }
    json["raw_path"] = raw_path_json;

    return json;
}

/// @brief Creates a pathfinder instance based on the algorithm name.
static std::unique_ptr<IPathfinder> create_pathfinder(const std::string& algorithm) {
    if (algorithm == "dijkstra") {
        return std::make_unique<Dijkstra>();
    }
    // Default: A*
    return std::make_unique<AStar>();
}

void RouteController::computeRoute(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {

    if (!g_graph) {
        Json::Value error;
        error["error"] = "Graph not loaded";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k503ServiceUnavailable);
        callback(resp);
        return;
    }

    // Parse request body
    auto json_body = req->getJsonObject();
    if (!json_body) {
        Json::Value error;
        error["error"] = "Invalid JSON body. Expected: {from_id, to_id, algorithm?}";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    if (!json_body->isMember("from_id") || !json_body->isMember("to_id")) {
        Json::Value error;
        error["error"] = "Missing required fields: from_id, to_id";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    uint64_t from_id = (*json_body)["from_id"].asUInt64();
    uint64_t to_id = (*json_body)["to_id"].asUInt64();
    std::string algorithm = "a_star";
    if (json_body->isMember("algorithm")) {
        algorithm = (*json_body)["algorithm"].asString();
    }

    // Validate station IDs
    if (!g_graph->has_station(from_id)) {
        Json::Value error;
        error["error"] = "Departure station not found";
        error["from_id"] = static_cast<Json::UInt64>(from_id);
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k404NotFound);
        callback(resp);
        return;
    }

    if (!g_graph->has_station(to_id)) {
        Json::Value error;
        error["error"] = "Arrival station not found";
        error["to_id"] = static_cast<Json::UInt64>(to_id);
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k404NotFound);
        callback(resp);
        return;
    }

    // Check cache first
    if (g_route_cache) {
        auto cached = g_route_cache->get(from_id, to_id, algorithm);
        if (cached.has_value()) {
            auto json = route_result_to_json(cached.value(), 0.0);
            json["cached"] = true;
            auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
            callback(resp);
            return;
        }
    }

    // Compute route
    auto pathfinder = create_pathfinder(algorithm);

    auto start_time = std::chrono::high_resolution_clock::now();
    auto result = pathfinder->compute(*g_graph, from_id, to_id);
    auto end_time = std::chrono::high_resolution_clock::now();

    double computation_ms = std::chrono::duration<double, std::milli>(
        end_time - start_time).count();

    // Cache the result
    if (g_route_cache && result.found) {
        g_route_cache->put(from_id, to_id, algorithm, result);
    }

    auto json = route_result_to_json(result, computation_ms);
    json["cached"] = false;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    if (!result.found) {
        resp->setStatusCode(drogon::k404NotFound);
    }
    callback(resp);
}

}  // namespace city_mapper
