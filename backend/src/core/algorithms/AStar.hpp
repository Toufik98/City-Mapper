#pragma once

/// @file AStar.hpp
/// @brief A* shortest-path algorithm with Haversine heuristic.

#include "IPathfinder.hpp"

namespace city_mapper {

/// @brief A* pathfinding with geographic distance heuristic.
/// @details Uses the Haversine formula to compute a straight-line distance
/// lower bound as the heuristic function h(n). This is admissible because
/// the straight-line distance / max_metro_speed never overestimates the actual travel time.
///
/// **Complexity:**
/// - Time:  O((V + E) log V) — same worst-case as Dijkstra, but typically
///   explores far fewer nodes due to heuristic guidance.
/// - Space: O(V) for distance, parent, and visited data structures.
///
/// **Fallback:** If stations lack geographic coordinates (latitude/longitude = 0),
/// the heuristic returns 0 and A* degrades gracefully to Dijkstra.
class AStar : public IPathfinder {
public:
    /// @brief Computes shortest path using A* with Haversine heuristic.
    /// @param graph The transit graph (stations must have lat/lng for optimal performance).
    /// @param start Source station ID.
    /// @param end Destination station ID.
    /// @return RouteResult with the shortest path. `found` is false if no path exists.
    [[nodiscard]] RouteResult compute(const Graph& graph,
                                      uint64_t start,
                                      uint64_t end) override;

    /// @brief Returns "A*".
    [[nodiscard]] std::string name() const override { return "A*"; }
};

}  // namespace city_mapper
