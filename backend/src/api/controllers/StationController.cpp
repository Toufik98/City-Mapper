/// @file StationController.cpp
/// @brief Implementation of station REST endpoints.

#include "StationController.hpp"

#include <json/json.h>

#include "../../core/graph/Graph.hpp"

namespace city_mapper {

extern Graph* g_graph;

/// @brief Serializes a Station to a JSON value.
static Json::Value station_to_json(const Station& station) {
    Json::Value json;
    json["id"] = static_cast<Json::UInt64>(station.id);
    json["name"] = station.name;
    json["line_id"] = station.line_id;
    json["line_name"] = station.line_name;
    json["address"] = station.address;
    json["latitude"] = station.latitude;
    json["longitude"] = station.longitude;
    return json;
}

void StationController::getAll(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {

    if (!g_graph) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            Json::Value("Graph not loaded"));
        resp->setStatusCode(drogon::k503ServiceUnavailable);
        callback(resp);
        return;
    }

    Json::Value json_array(Json::arrayValue);
    for (const auto& [id, station] : g_graph->get_stations()) {
        json_array.append(station_to_json(station));
    }

    Json::Value response;
    response["count"] = static_cast<Json::UInt64>(g_graph->station_count());
    response["stations"] = json_array;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

void StationController::getById(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    uint64_t id) {

    if (!g_graph) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            Json::Value("Graph not loaded"));
        resp->setStatusCode(drogon::k503ServiceUnavailable);
        callback(resp);
        return;
    }

    auto station = g_graph->get_station(id);
    if (!station.has_value()) {
        Json::Value error;
        error["error"] = "Station not found";
        error["id"] = static_cast<Json::UInt64>(id);
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k404NotFound);
        callback(resp);
        return;
    }

    auto resp = drogon::HttpResponse::newHttpJsonResponse(
        station_to_json(station.value()));
    callback(resp);
}

void StationController::search(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {

    if (!g_graph) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            Json::Value("Graph not loaded"));
        resp->setStatusCode(drogon::k503ServiceUnavailable);
        callback(resp);
        return;
    }

    std::string query = req->getParameter("q");
    if (query.empty()) {
        Json::Value error;
        error["error"] = "Missing required query parameter 'q'";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    size_t limit = 10;
    std::string limit_str = req->getParameter("limit");
    if (!limit_str.empty()) {
        limit = std::strtoull(limit_str.c_str(), nullptr, 10);
        if (limit == 0 || limit > 100) {
            limit = 10;
        }
    }

    auto results = g_graph->search_stations(query, limit);

    Json::Value json_array(Json::arrayValue);
    for (const auto& station : results) {
        json_array.append(station_to_json(station));
    }

    Json::Value response;
    response["query"] = query;
    response["count"] = static_cast<Json::UInt64>(results.size());
    response["stations"] = json_array;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
    callback(resp);
}

}  // namespace city_mapper
