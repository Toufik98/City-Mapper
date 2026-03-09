#pragma once

/// @file RouteController.hpp
/// @brief Route computation endpoint: POST /api/route

#include <drogon/HttpController.h>

namespace city_mapper {

class RouteController : public drogon::HttpController<RouteController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(RouteController::computeRoute, "/api/route", drogon::Post,
                drogon::Options);
  METHOD_LIST_END

  /// @brief Computes the shortest route between two stations.
  /// @details POST /api/route
  /// Request body:
  /// ```json
  /// {
  ///   "from_id": 1642,
  ///   "to_id": 1648,
  ///   "algorithm": "dijkstra"  // "dijkstra" | "a_star" (default: "a_star")
  /// }
  /// ```
  /// Response:
  /// ```json
  /// {
  ///   "found": true,
  ///   "algorithm": "A*",
  ///   "total_duration_seconds": 1230,
  ///   "total_duration_display": "20 min",
  ///   "total_transfers": 2,
  ///   "total_stations": 15,
  ///   "segments": [...]
  /// }
  /// ```
  void
  computeRoute(const drogon::HttpRequestPtr &req,
               std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};

} // namespace city_mapper
