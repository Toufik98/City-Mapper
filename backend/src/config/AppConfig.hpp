#pragma once

/// @file AppConfig.hpp
/// @brief Environment-based application configuration with automatic path
/// resolution.

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <string>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace city_mapper {

namespace detail {

/// @brief Returns the directory containing the running executable.
inline std::filesystem::path executable_dir() {
#ifdef __APPLE__
  char buf[4096];
  uint32_t size = sizeof(buf);
  if (_NSGetExecutablePath(buf, &size) == 0) {
    return std::filesystem::canonical(buf).parent_path();
  }
#elif defined(__linux__)
  auto exe = std::filesystem::canonical("/proc/self/exe");
  return exe.parent_path();
#endif
  return std::filesystem::current_path();
}

/// @brief Walks up from `start` looking for a directory that contains
///        `data/csv/s.csv` — this identifies the project root.
/// @returns The project root path, or empty path if not found.
inline std::filesystem::path
find_project_root(const std::filesystem::path &start) {
  namespace fs = std::filesystem;
  auto dir = start;

  // Walk up at most 10 levels
  for (int i = 0; i < 10; ++i) {
    if (fs::exists(dir / "data" / "s.csv")) {
      return dir;
    }
    auto parent = dir.parent_path();
    if (parent == dir)
      break; // reached filesystem root
    dir = parent;
  }
  return {};
}

} // namespace detail

/// @brief Application configuration loaded from environment variables.
/// @details All settings have sensible defaults for development. Path
///          resolution order:
///          1. Environment variables (DATA_PATH, GTFS_PATH, etc.)
///          2. Compile-time PROJECT_ROOT_DIR (set by CMake)
///          3. Walk up from executable location to find project root
///          4. Relative paths (original fallback)
struct AppConfig {
  std::string data_path = "data";
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
  ///   - DATA_PATH: Base directory for CSV data files
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

    // ── Resolve project root for default paths ──────────────
    bool data_path_from_env = false;
    bool gtfs_path_from_env = false;

    if (const char *val = std::getenv("DATA_PATH")) {
      config.data_path = val;
      data_path_from_env = true;
    }
    if (const char *val = std::getenv("STATIONS_FILE")) {
      config.stations_file = val;
    }
    if (const char *val = std::getenv("CONNECTIONS_FILE")) {
      config.connections_file = val;
    }
    if (const char *val = std::getenv("GTFS_PATH")) {
      config.gtfs_path = val;
      gtfs_path_from_env = true;
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

    // If neither DATA_PATH nor GTFS_PATH was set via env, try to auto-resolve
    if (!data_path_from_env || !gtfs_path_from_env) {
      auto root = resolve_project_root();
      if (!root.empty()) {
        if (!data_path_from_env) {
          config.data_path = (root / "data").string();
        }
        if (!gtfs_path_from_env) {
          config.gtfs_path = (root / "data" / "IDFM-gtfs").string();
        }
      }
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

private:
  /// @brief Resolves the project root directory.
  /// @details Resolution order:
  ///   1. Compile-time PROJECT_ROOT_DIR (set by CMake)
  ///   2. Walk up from executable directory
  static std::filesystem::path resolve_project_root() {
    namespace fs = std::filesystem;

    // Try compile-time project root first
#ifdef PROJECT_ROOT_DIR
    {
      fs::path compile_root(PROJECT_ROOT_DIR);
      if (fs::exists(compile_root / "data" / "s.csv")) {
        return compile_root;
      }
    }
#endif

    // Walk up from executable location
    auto root = detail::find_project_root(detail::executable_dir());
    if (!root.empty()) {
      return root;
    }

    // Walk up from current working directory as last resort
    return detail::find_project_root(fs::current_path());
  }
};

} // namespace city_mapper
