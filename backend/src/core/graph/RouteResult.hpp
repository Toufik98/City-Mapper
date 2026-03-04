#pragma once

/// @file RouteResult.hpp
/// @brief Data structures for pathfinding results.

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "Connection.hpp"
#include "Station.hpp"

namespace city_mapper {

/// @brief A single segment of a computed route (one metro line or transfer).
struct RouteSegment {
    Station from_station;
    Station to_station;
    uint64_t duration_seconds{0};
    ConnectionType type{ConnectionType::RIDE};
    std::string line_id;
    std::string line_name;
};

/// @brief Complete result of a pathfinding computation.
struct RouteResult {
    std::vector<RouteSegment> segments;
    uint64_t total_duration_seconds{0};
    uint32_t total_transfers{0};
    uint32_t total_stations{0};
    std::string algorithm;
    bool found{false};

    /// @brief Raw path as (station_id, cumulative_cost_seconds) pairs.
    /// @details Preserved for backward compatibility and debugging.
    std::vector<std::pair<uint64_t, uint64_t>> raw_path;
};

}  // namespace city_mapper
