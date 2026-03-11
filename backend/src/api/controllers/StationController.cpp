/// @file StationController.cpp
/// @brief Implementation of station REST endpoints.

#include "StationController.hpp"

#include <json/json.h>

#include "../../core/graph/Graph.hpp"

namespace city_mapper {

extern Graph *g_graph;

/// @brief Serializes a Station to a JSON value.
static Json::Value station_to_json(const Station &station) {
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
    const drogon::HttpRequestPtr & /*req*/,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

  if (!g_graph) {
    auto resp = drogon::HttpResponse::newHttpJsonResponse(
        Json::Value("Graph not loaded"));
    resp->setStatusCode(drogon::k503ServiceUnavailable);
    callback(resp);
    return;
  }

  Json::Value json_array(Json::arrayValue);
  for (const auto &[id, station] : g_graph->get_stations()) {
    json_array.append(station_to_json(station));
  }

  Json::Value response;
  response["count"] = static_cast<Json::UInt64>(g_graph->station_count());
  response["stations"] = json_array;

  auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
  callback(resp);
}

void StationController::getById(
    const drogon::HttpRequestPtr & /*req*/,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
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
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

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
  for (const auto &result : results) {
    Json::Value json = station_to_json(result.station);

    // Add deduplicated lines array
    Json::Value lines_array(Json::arrayValue);
    for (const auto &line : result.lines) {
      lines_array.append(line);
    }
    json["lines"] = lines_array;

    json_array.append(json);
  }

  Json::Value response;
  response["query"] = query;
  response["count"] = static_cast<Json::UInt64>(results.size());
  response["stations"] = json_array;

  auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
  callback(resp);
}

void StationController::getConnections(
    const drogon::HttpRequestPtr & /*req*/,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

  if (!g_graph) {
    auto resp = drogon::HttpResponse::newHttpJsonResponse(
        Json::Value("Graph not loaded"));
    resp->setStatusCode(drogon::k503ServiceUnavailable);
    callback(resp);
    return;
  }

  Json::Value json_array(Json::arrayValue);
  for (const auto &[from_id, edges] : g_graph->get_adjacency()) {
    auto from_station = g_graph->get_station(from_id);
    if (!from_station || !from_station->has_coordinates())
      continue;

    for (const auto &edge : edges) {
      auto to_station = g_graph->get_station(edge.target_id);
      if (!to_station || !to_station->has_coordinates())
        continue;

      // Only draw RIDE connections for metro lines
      if (edge.type != ConnectionType::RIDE)
        continue;

      Json::Value conn;
      conn["from_id"] = static_cast<Json::UInt64>(from_id);
      conn["to_id"] = static_cast<Json::UInt64>(edge.target_id);
      conn["from_lat"] = from_station->latitude;
      conn["from_lon"] = from_station->longitude;
      conn["to_lat"] = to_station->latitude;
      conn["to_lon"] = to_station->longitude;
      conn["line_id"] = from_station->line_id;

      json_array.append(conn);
    }
  }

  Json::Value response;
  response["count"] = static_cast<Json::UInt64>(json_array.size());
  response["connections"] = json_array;

  auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
  callback(resp);
}

} // namespace city_mapper
