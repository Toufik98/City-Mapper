#pragma once

/// @file TimeFormatter.hpp
/// @brief Utility for converting seconds to human-readable time strings.

#include <cstdint>
#include <string>

namespace city_mapper {

/// @brief Formats a duration in seconds to a human-readable string.
/// @param seconds Duration in seconds.
/// @return Formatted string, e.g., "1h 23min 45s", "23min 45s", "45s".
inline std::string format_duration(uint64_t seconds) {
    if (seconds == 0) {
        return "0s";
    }

    std::string result;

    uint64_t hours = seconds / 3600;
    uint64_t minutes = (seconds % 3600) / 60;
    uint64_t secs = seconds % 60;

    if (hours > 0) {
        result += std::to_string(hours) + "h ";
    }
    if (minutes > 0 || hours > 0) {
        result += std::to_string(minutes) + "min ";
    }
    if (secs > 0 || result.empty()) {
        result += std::to_string(secs) + "s";
    }

    // Trim trailing space
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }

    return result;
}

/// @brief Formats a duration in seconds to a compact "Xmin" format.
/// @param seconds Duration in seconds.
/// @return Formatted string, e.g., "23 min" or "< 1 min".
inline std::string format_duration_compact(uint64_t seconds) {
    if (seconds < 60) {
        return "< 1 min";
    }
    uint64_t minutes = (seconds + 30) / 60;  // Round to nearest minute
    return std::to_string(minutes) + " min";
}

}  // namespace city_mapper
