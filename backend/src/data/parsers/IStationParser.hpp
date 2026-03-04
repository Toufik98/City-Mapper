#pragma once

/// @file IStationParser.hpp
/// @brief Interface for station data parsers (Strategy pattern).

#include <string>
#include <unordered_map>

#include "../core/graph/Station.hpp"

namespace city_mapper {

/// @brief Abstract interface for parsing station data from various sources.
/// @details Implementations: CSVStationParser, GTFSParser (future).
/// Follows the Strategy pattern — callers depend on this interface,
/// never on concrete parser implementations.
class IStationParser {
public:
    virtual ~IStationParser() = default;

    /// @brief Parses station data from the given file.
    /// @param filename Path to the data file.
    /// @return Map of station_id → Station.
    /// @throws std::runtime_error if the file cannot be read or parsed.
    [[nodiscard]] virtual std::unordered_map<uint64_t, Station> parse(
        const std::string& filename) const = 0;
};

}  // namespace city_mapper
