#pragma once

/// @file StationController.hpp
/// @brief Station endpoints: GET /api/stations, GET /api/stations/{id}, GET
/// /api/stations/search

#include <drogon/HttpController.h>

namespace city_mapper {

class StationController : public drogon::HttpController<StationController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(StationController::getAll, "/api/stations", drogon::Get);
  ADD_METHOD_TO(StationController::getById, "/api/stations/{id}", drogon::Get);
  ADD_METHOD_TO(StationController::search, "/api/stations/search", drogon::Get);
  ADD_METHOD_TO(StationController::getConnections, "/api/connections",
                drogon::Get);
  METHOD_LIST_END

  /// @brief Returns all connections between stations.
  /// @details GET /api/connections
  void getConnections(
      const drogon::HttpRequestPtr &req,
      std::function<void(const drogon::HttpResponsePtr &)> &&callback);

  /// @brief Returns all stations in the graph.
  /// @details GET /api/stations
  void getAll(const drogon::HttpRequestPtr &req,
              std::function<void(const drogon::HttpResponsePtr &)> &&callback);

  /// @brief Returns a single station by ID.
  /// @details GET /api/stations/{id}
  void getById(const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback,
               uint64_t id);

  /// @brief Searches stations by name (fuzzy, case-insensitive).
  /// @details GET /api/stations/search?q=chatelet&limit=10
  void search(const drogon::HttpRequestPtr &req,
              std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

} // namespace city_mapper
