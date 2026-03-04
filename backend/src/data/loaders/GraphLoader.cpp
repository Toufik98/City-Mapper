/// @file GraphLoader.cpp
/// @brief Implementation of the graph loader — orchestrates parsing and graph construction.

#include "GraphLoader.hpp"

#include <stdexcept>
#include <utility>

namespace city_mapper {

GraphLoader::GraphLoader(std::unique_ptr<IStationParser> station_parser,
                         std::unique_ptr<IConnectionParser> connection_parser)
    : station_parser_(std::move(station_parser)),
      connection_parser_(std::move(connection_parser)) {
    if (!station_parser_) {
        throw std::invalid_argument("GraphLoader: station_parser must not be null");
    }
    if (!connection_parser_) {
        throw std::invalid_argument("GraphLoader: connection_parser must not be null");
    }
}

Graph GraphLoader::load(const std::string& stations_file,
                        const std::string& connections_file) const {
    // Phase 1: Parse stations
    auto stations = station_parser_->parse(stations_file);

    // Phase 2: Parse connections
    auto connections = connection_parser_->parse(connections_file);

    // Phase 3: Build graph
    Graph graph;

    // Add all stations to the graph
    for (auto& [id, station] : stations) {
        graph.add_station(id, std::move(station));
    }

    // Add all connections as directed edges
    for (const auto& [from_id, neighbors] : connections) {
        for (const auto& [to_id, weight] : neighbors) {
            // Classify the connection type
            ConnectionType type = ConnectionType::RIDE;
            auto from_opt = graph.get_station(from_id);
            auto to_opt = graph.get_station(to_id);

            if (from_opt.has_value() && to_opt.has_value()) {
                type = classify_connection(weight, from_opt.value(), to_opt.value());
            }

            graph.add_connection(from_id, to_id, weight, type);
        }
    }

    return graph;
}

ConnectionType GraphLoader::classify_connection(uint64_t weight,
                                                const Station& from,
                                                const Station& to) {
    // Transfer: same station name but different line (weight often 0 or very small)
    // In the RATP data, transfers between lines at the same physical station
    // have weight = 0 (same-platform) or small values (walking between platforms).
    if (from.name == to.name && from.line_id != to.line_id) {
        return ConnectionType::TRANSFER;
    }

    // Zero-weight connections on the same line are typically direction changes
    // (aller/retour at the same station) — classify as transfer
    if (weight == 0) {
        return ConnectionType::TRANSFER;
    }

    // Default: regular ride between consecutive stations
    return ConnectionType::RIDE;
}

}  // namespace city_mapper
