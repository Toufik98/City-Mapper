#pragma once

/// @file AppConfig.hpp
/// @brief Environment-based application configuration.

#include <cstddef>
#include <cstdlib>
#include <string>

namespace city_mapper {

/// @brief Application configuration loaded from environment variables.
/// @details All settings have sensible defaults for development.
struct AppConfig {
  std::string data_path = "data/csv";
  std::string stations_file = "s.csv";
  std::string connections_file = "c.csv";
  std::string gtfs_path = "data/IDFM-gtfs";
  std::string gtfs_stops_file = "stops.txt";
  std::string host = "0.0.0.0";
  uint16_t port = 8080;
  size_t cache_size = 10'000;
  size_t thread_count = 4;

  /// @brief Loads configuration from environment variables.
  /// @details Environment variables:
  ///   - DATA_PATH: Base directory for data files
  ///   - STATIONS_FILE: Stations CSV filename
  ///   - CONNECTIONS_FILE: Connections CSV filename
  ///   - GTFS_PATH: Base directory for GTFS data files
  ///   - GTFS_STOPS_FILE: GTFS stops filename
  ///   - HOST: Server bind address
  ///   - PORT: Server listen port
  ///   - CACHE_SIZE: Route cache capacity
  ///   - THREAD_COUNT: Drogon I/O thread count
  static AppConfig from_env() {
    AppConfig config;

    if (const char *val = std::getenv("DATA_PATH")) {
      config.data_path = val;
    }
    if (const char *val = std::getenv("STATIONS_FILE")) {
      config.stations_file = val;
    }
    if (const char *val = std::getenv("CONNECTIONS_FILE")) {
      config.connections_file = val;
    }
    if (const char *val = std::getenv("GTFS_PATH")) {
      config.gtfs_path = val;
    }
    if (const char *val = std::getenv("GTFS_STOPS_FILE")) {
      config.gtfs_stops_file = val;
    }
    if (const char *val = std::getenv("HOST")) {
      config.host = val;
    }
    if (const char *val = std::getenv("PORT")) {
      config.port = static_cast<uint16_t>(std::strtoul(val, nullptr, 10));
    }
    if (const char *val = std::getenv("CACHE_SIZE")) {
      config.cache_size = std::strtoull(val, nullptr, 10);
    }
    if (const char *val = std::getenv("THREAD_COUNT")) {
      config.thread_count = std::strtoull(val, nullptr, 10);
    }

    return config;
  }

  /// @brief Returns the full path to the stations file.
  [[nodiscard]] std::string stations_path() const {
    return data_path + "/" + stations_file;
  }

  /// @brief Returns the full path to the connections file.
  [[nodiscard]] std::string connections_path() const {
    return data_path + "/" + connections_file;
  }

  /// @brief Returns the full path to the GTFS stops file.
  [[nodiscard]] std::string gtfs_stops_path() const {
    return gtfs_path + "/" + gtfs_stops_file;
  }
};

} // namespace city_mapper
