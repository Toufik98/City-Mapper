#pragma once

/// @file Dijkstra.hpp
/// @brief Optimized Dijkstra's shortest-path algorithm using a min-heap.

#include "IPathfinder.hpp"

namespace city_mapper {

/// @brief Dijkstra's algorithm with std::priority_queue (min-heap).
/// @details Computes the single-source shortest path from start to end.
///
/// **Complexity:**
/// - Time:  O((V + E) log V) with binary heap priority queue
/// - Space: O(V) for distance map + parent map + visited set
///
/// **Improvements over the original implementation:**
/// - Uses `std::priority_queue` with `std::greater` (min-heap) instead of
///   a sorted vector — O(log V) insertion/extraction vs O(V log V) re-sort.
/// - Uses `std::unordered_set` for visited nodes — O(1) lookup vs O(V) linear scan.
/// - Uses `std::unordered_map` for distances with infinity initialization on-demand
///   (lazy initialization) instead of pre-allocating for all nodes.
class Dijkstra : public IPathfinder {
public:
    /// @brief Computes shortest path using optimized Dijkstra.
    /// @param graph The transit graph.
    /// @param start Source station ID.
    /// @param end Destination station ID.
    /// @return RouteResult with the shortest path. `found` is false if no path exists.
    [[nodiscard]] RouteResult compute(const Graph& graph,
                                      uint64_t start,
                                      uint64_t end) override;

    /// @brief Returns "Dijkstra".
    [[nodiscard]] std::string name() const override { return "Dijkstra"; }
};

}  // namespace city_mapper
