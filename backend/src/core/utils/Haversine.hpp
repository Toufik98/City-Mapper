#pragma once

/// @file Haversine.hpp
/// @brief Geographic distance calculation using the Haversine formula.

#include <cmath>
#include <cstdint>
#include <numbers>

namespace city_mapper {

/// @brief Computes the great-circle distance between two geographic points.
/// @details Uses the Haversine formula for distance on a sphere.
/// This is an admissible heuristic for A* on geographic graphs
/// (never overestimates the actual travel distance).
///
/// **Formula:**
/// a = sin²(Δlat/2) + cos(lat1) · cos(lat2) · sin²(Δlng/2)
/// c = 2 · atan2(√a, √(1−a))
/// d = R · c
///
/// @param lat1 Latitude of point 1 in degrees.
/// @param lon1 Longitude of point 1 in degrees.
/// @param lat2 Latitude of point 2 in degrees.
/// @param lon2 Longitude of point 2 in degrees.
/// @return Distance in meters.
inline double haversine_distance(double lat1, double lon1, double lat2, double lon2) noexcept {
    constexpr double EARTH_RADIUS_METERS = 6'371'000.0;
    constexpr double DEG_TO_RAD = std::numbers::pi / 180.0;

    double dlat = (lat2 - lat1) * DEG_TO_RAD;
    double dlon = (lon2 - lon1) * DEG_TO_RAD;

    double lat1_rad = lat1 * DEG_TO_RAD;
    double lat2_rad = lat2 * DEG_TO_RAD;

    double a = std::sin(dlat / 2.0) * std::sin(dlat / 2.0) +
               std::cos(lat1_rad) * std::cos(lat2_rad) *
               std::sin(dlon / 2.0) * std::sin(dlon / 2.0);

    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));

    return EARTH_RADIUS_METERS * c;
}

/// @brief Estimates travel time in seconds based on geographic distance.
/// @details Uses a conservative average metro speed to ensure admissibility.
/// Paris Metro average speed ≈ 20-25 km/h including stops.
/// We use 40 km/h (upper bound) to ensure the heuristic never overestimates.
///
/// @param distance_meters Distance in meters.
/// @return Estimated travel time in seconds (lower bound).
inline uint64_t estimate_travel_time_seconds(double distance_meters) noexcept {
    constexpr double MAX_METRO_SPEED_MPS = 40'000.0 / 3600.0;  // 40 km/h → m/s ≈ 11.11 m/s
    if (distance_meters <= 0.0) {
        return 0;
    }
    return static_cast<uint64_t>(distance_meters / MAX_METRO_SPEED_MPS);
}

}  // namespace city_mapper
