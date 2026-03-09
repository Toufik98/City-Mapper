/// @file main.cpp
/// @brief City-Mapper API server entry point.
/// @details Loads the transit graph from CSV data, configures Drogon,
/// and starts the HTTP server.

#include <drogon/drogon.h>

#include <iostream>
#include <memory>

#include "config/AppConfig.hpp"
#include "core/cache/RouteCache.hpp"
#include "core/graph/Graph.hpp"
#include "data/loaders/GraphLoader.hpp"
#include "data/parsers/CSVConnectionParser.hpp"
#include "data/parsers/CSVStationParser.hpp"
#include "data/parsers/GTFSStopsParser.hpp"

namespace city_mapper {

// Global application state — accessed by controllers via extern declarations.
// Owned by main(), lifetime = application lifetime.
Graph *g_graph = nullptr;
RouteCache *g_route_cache = nullptr;

} // namespace city_mapper

int main() {
  using namespace city_mapper;

  std::cout << "=== City-Mapper API Server ===" << std::endl;

  // Load configuration from environment
  auto config = AppConfig::from_env();

  std::cout << "[config] Data path: " << config.data_path << std::endl;
  std::cout << "[config] Stations:  " << config.stations_file << std::endl;
  std::cout << "[config] Connections: " << config.connections_file << std::endl;
  std::cout << "[config] GTFS path: " << config.gtfs_path << std::endl;
  std::cout << "[config] GTFS stops: " << config.gtfs_stops_file << std::endl;
  std::cout << "[config] Listen:    " << config.host << ":" << config.port
            << std::endl;
  std::cout << "[config] Cache:     " << config.cache_size << " entries"
            << std::endl;
  std::cout << "[config] Threads:   " << config.thread_count << std::endl;

  // Load transit graph
  std::cout << "\n[init] Loading transit graph..." << std::endl;

  try {
    GraphLoader loader(std::make_unique<CSVStationParser>(),
                       std::make_unique<CSVConnectionParser>());

    static Graph graph =
        loader.load(config.stations_path(), config.connections_path());
    g_graph = &graph;

    std::cout << "[init] Graph loaded: " << graph.station_count()
              << " stations, " << graph.connection_count() << " connections"
              << std::endl;

    // Merge coordinates from GTFS stops if available
    try {
      GTFSStopsParser gtfs_parser;
      auto stops_coords = gtfs_parser.parse(config.gtfs_stops_path());

      size_t updated = 0;
      std::vector<Station> to_update;
      for (const auto &[id, station] : graph.get_stations()) {
        std::string norm_name = GTFSStopsParser::normalizeName(station.name);
        if (stops_coords.count(norm_name)) {
          Station copy = station;
          copy.latitude = stops_coords[norm_name].first;
          copy.longitude = stops_coords[norm_name].second;
          to_update.push_back(copy);
          updated++;
        }
      }

      for (const auto &station : to_update) {
        graph.add_station(station.id, station);
      }
      std::cout << "[init] Merged GTFS coordinates for " << updated
                << " stations." << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "[warn] Failed to load GTFS coordinates: " << e.what()
                << std::endl;
    }
  } catch (const std::exception &e) {
    std::cerr << "[error] Failed to load graph: " << e.what() << std::endl;
    return 1;
  }

  // Initialize route cache
  static RouteCache route_cache(config.cache_size);
  g_route_cache = &route_cache;

  std::cout << "[init] Route cache initialized (" << config.cache_size
            << " entries capacity)" << std::endl;

  // Configure Drogon
  drogon::app()
      .setLogPath("./logs")
      .setLogLevel(trantor::Logger::kInfo)
      .addListener(config.host, config.port)
      .setThreadNum(static_cast<size_t>(config.thread_count))
      .enableGzip(true)
      // Add CORS headers to all responses
      .registerPostHandlingAdvice([](const drogon::HttpRequestPtr & /*req*/,
                                     const drogon::HttpResponsePtr &resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers",
                        "Content-Type, Authorization, Accept");
      });

  std::cout << "\n[server] Starting City-Mapper API on " << config.host << ":"
            << config.port << "..." << std::endl;
  std::cout << "[server] Endpoints:" << std::endl;
  std::cout << "  GET  /health                - Service health check"
            << std::endl;
  std::cout << "  GET  /api/stations          - List all stations" << std::endl;
  std::cout << "  GET  /api/stations/{id}     - Get station by ID" << std::endl;
  std::cout << "  GET  /api/stations/search?q= - Search stations" << std::endl;
  std::cout << "  POST /api/route             - Compute shortest route"
            << std::endl;
  std::cout << std::endl;

  drogon::app().run();

  return 0;
}
