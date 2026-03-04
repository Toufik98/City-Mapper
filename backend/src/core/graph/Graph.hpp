#pragma once

/// @file Graph.hpp
/// @brief Adjacency-list graph representation for the transit network.
/// @details O(1) station lookup, O(1) neighbor access via unordered_map.

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Connection.hpp"
#include "Station.hpp"

namespace city_mapper {

/// @brief Weighted directed graph representing the metro network.
/// @details Stations are nodes, connections are directed weighted edges.
///
/// **Complexity guarantees:**
/// - Station lookup by ID: O(1) amortized
/// - Neighbor access: O(1) amortized
/// - Station search by name: O(n) where n = number of stations
/// - Add station/connection: O(1) amortized
class Graph {
public:
    /// @brief Adds a station to the graph. Overwrites if ID already exists.
    /// @param id Unique station identifier.
    /// @param station Station data.
    void add_station(uint64_t id, Station station);

    /// @brief Adds a directed edge from one station to another.
    /// @param from_id Source station ID.
    /// @param to_id Target station ID.
    /// @param weight Travel time in seconds.
    /// @param type Connection type (ride, transfer, walk).
    void add_connection(uint64_t from_id, uint64_t to_id, uint64_t weight,
                        ConnectionType type = ConnectionType::RIDE);

    /// @brief Looks up a station by ID.
    /// @param id The station ID.
    /// @return The station if found, std::nullopt otherwise.
    [[nodiscard]] std::optional<Station> get_station(uint64_t id) const;

    /// @brief Returns a const reference to the full stations map.
    [[nodiscard]] const std::unordered_map<uint64_t, Station>& get_stations() const noexcept;

    /// @brief Returns all outgoing edges from a given station.
    /// @param station_id The source station ID.
    /// @return Vector of weighted edges. Empty vector if station has no connections.
    [[nodiscard]] const std::vector<WeightedEdge>& get_neighbors(uint64_t station_id) const;

    /// @brief Checks whether a station exists in the graph.
    [[nodiscard]] bool has_station(uint64_t id) const noexcept;

    /// @brief Returns the total number of stations.
    [[nodiscard]] size_t station_count() const noexcept;

    /// @brief Returns the total number of directed connections.
    [[nodiscard]] size_t connection_count() const noexcept;

    /// @brief Searches for stations whose name contains the query (case-insensitive).
    /// @param query Search string.
    /// @param limit Maximum number of results.
    /// @return Vector of matching stations, ordered by relevance (exact prefix match first).
    [[nodiscard]] std::vector<Station> search_stations(const std::string& query,
                                                       size_t limit = 10) const;

    /// @brief Returns the raw adjacency list for advanced operations.
    [[nodiscard]] const std::unordered_map<uint64_t, std::vector<WeightedEdge>>&
    get_adjacency() const noexcept;

private:
    std::unordered_map<uint64_t, Station> stations_;
    std::unordered_map<uint64_t, std::vector<WeightedEdge>> adjacency_;
    size_t total_connections_{0};

    /// Sentinel returned when a station has no outgoing edges.
    static const std::vector<WeightedEdge> EMPTY_NEIGHBORS;
};

}  // namespace city_mapper
