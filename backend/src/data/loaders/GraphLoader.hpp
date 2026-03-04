#pragma once

/// @file GraphLoader.hpp
/// @brief Orchestrates data parsing and Graph construction.

#include <memory>
#include <string>

#include "core/graph/Graph.hpp"
#include "data/parsers/IConnectionParser.hpp"
#include "data/parsers/IStationParser.hpp"

namespace city_mapper {

/// @brief Loads station and connection data into a Graph.
/// @details Depends on parser abstractions (IStationParser, IConnectionParser)
/// following the Dependency Inversion Principle. The concrete parser
/// implementations are injected at construction time.
class GraphLoader {
public:
    /// @brief Constructs a GraphLoader with the given parsers.
    /// @param station_parser Parser for station data.
    /// @param connection_parser Parser for connection data.
    GraphLoader(std::unique_ptr<IStationParser> station_parser,
                std::unique_ptr<IConnectionParser> connection_parser);

    /// @brief Loads data from the given files and builds a Graph.
    /// @param stations_file Path to the stations data file.
    /// @param connections_file Path to the connections data file.
    /// @return Fully constructed Graph ready for pathfinding.
    /// @throws std::runtime_error if files cannot be read or parsed.
    [[nodiscard]] Graph load(const std::string& stations_file,
                             const std::string& connections_file) const;

private:
    std::unique_ptr<IStationParser> station_parser_;
    std::unique_ptr<IConnectionParser> connection_parser_;

    /// @brief Detects connection type based on weight and station data.
    /// @details Transfer connections (weight = 0 or same station name, different line)
    /// are classified as TRANSFER. All others are classified as RIDE.
    static ConnectionType classify_connection(uint64_t weight,
                                              const Station& from,
                                              const Station& to);
};

}  // namespace city_mapper
