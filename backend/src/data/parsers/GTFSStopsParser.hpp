#pragma once

#include <string>
#include <unordered_map>
#include <utility>

namespace city_mapper {

/**
 * @class GTFSStopsParser
 * @brief Parses GTFS stops.txt to extract latitude and longitude for stations.
 */
class GTFSStopsParser {
public:
  /**
   * @brief Parses the specified GTFS stops.txt file.
   * @param filename Path to the stops.txt file.
   * @return A map from normalized station name to a pair of {latitude,
   * longitude}.
   */
  std::unordered_map<std::string, std::pair<double, double>>
  parse(const std::string &filename) const;

  /**
   * @brief Normalizes a station name for easier matching.
   * Removes accents, transforms to lowercase, and trims spaces.
   */
  static std::string normalizeName(const std::string &name);
};

} // namespace city_mapper
