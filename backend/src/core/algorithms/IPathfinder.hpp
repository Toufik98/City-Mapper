#pragma once

/// @file IPathfinder.hpp
/// @brief Abstract interface for pathfinding algorithms (Strategy pattern).

#include <cstdint>
#include <string>

#include "../graph/Graph.hpp"
#include "../graph/RouteResult.hpp"

namespace city_mapper {

/// @brief Strategy interface for pathfinding algorithms.
/// @details Implementations: Dijkstra, AStar, ContractionHierarchies, ParetoRouter.
/// Each algorithm provides the same interface but with different performance
/// characteristics and optimality guarantees.
class IPathfinder {
public:
    virtual ~IPathfinder() = default;

    /// @brief Computes the shortest path between two stations.
    /// @param graph The transit graph.
    /// @param start Source station ID.
    /// @param end Destination station ID.
    /// @return RouteResult with path details. Check `found` to see if a path exists.
    [[nodiscard]] virtual RouteResult compute(const Graph& graph,
                                              uint64_t start,
                                              uint64_t end) = 0;

    /// @brief Returns the algorithm name (for display / logging).
    [[nodiscard]] virtual std::string name() const = 0;
};

}  // namespace city_mapper
