#pragma once

/// @file Connection.hpp
/// @brief Edge models for the transit graph — connections between stations.

#include <cstdint>
#include <string>

namespace city_mapper {

/// @brief Type of connection between two stations.
enum class ConnectionType : uint8_t {
    RIDE,      ///< Normal metro ride between consecutive stations on the same line
    TRANSFER,  ///< Transfer between lines at the same physical station (walking inside)
    WALK       ///< Walking connection between nearby but distinct stations
};

/// @brief Converts ConnectionType enum to its string representation.
/// @param type The connection type.
/// @return Null-terminated string literal.
constexpr const char* connection_type_to_string(ConnectionType type) noexcept {
    switch (type) {
        case ConnectionType::RIDE:     return "RIDE";
        case ConnectionType::TRANSFER: return "TRANSFER";
        case ConnectionType::WALK:     return "WALK";
    }
    return "UNKNOWN";
}

/// @brief Full connection record (edge between two nodes).
struct Connection {
    uint64_t from_id{0};
    uint64_t to_id{0};
    uint64_t weight_seconds{0};
    ConnectionType type{ConnectionType::RIDE};

    bool operator==(const Connection& other) const noexcept {
        return from_id == other.from_id && to_id == other.to_id;
    }
};

/// @brief Lightweight edge stored in adjacency lists.
/// @details Only stores the target and weight — the source is the map key.
struct WeightedEdge {
    uint64_t target_id{0};
    uint64_t weight_seconds{0};
    ConnectionType type{ConnectionType::RIDE};
};

}  // namespace city_mapper
