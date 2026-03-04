#pragma once

/// @file Station.hpp
/// @brief Station model representing a metro station node in the transit graph.

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>

namespace city_mapper {

/// @brief Represents a metro station with geographic coordinates.
/// @details Maps to a node in the weighted directed graph.
/// Coordinates enable A* heuristic computation via Haversine distance.
struct Station {
    uint64_t id{0};
    std::string name;
    std::string line_id;
    std::string line_name;
    std::string address;
    double latitude{0.0};   ///< WGS84 latitude  (from GTFS stops.txt)
    double longitude{0.0};  ///< WGS84 longitude (from GTFS stops.txt)

    /// @brief Equality based on station ID only.
    bool operator==(const Station& other) const noexcept {
        return id == other.id;
    }

    bool operator!=(const Station& other) const noexcept {
        return !(*this == other);
    }

    /// @brief Returns true if this station has valid geographic coordinates.
    [[nodiscard]] bool has_coordinates() const noexcept {
        return latitude != 0.0 || longitude != 0.0;
    }

    friend std::ostream& operator<<(std::ostream& os, const Station& station) {
        os << "Station{id=" << station.id
           << ", name=\"" << station.name
           << "\", line=" << station.line_id << "}";
        return os;
    }
};

}  // namespace city_mapper

// Hash specialization for use in std::unordered_map / std::unordered_set
namespace std {
template <>
struct hash<city_mapper::Station> {
    size_t operator()(const city_mapper::Station& s) const noexcept {
        return hash<uint64_t>{}(s.id);
    }
};
}  // namespace std
