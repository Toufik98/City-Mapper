#pragma once

/// @file IConnectionParser.hpp
/// @brief Interface for connection data parsers (Strategy pattern).

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../core/graph/Connection.hpp"

namespace city_mapper {

/// @brief Abstract interface for parsing connection/edge data.
/// @details Implementations: CSVConnectionParser, GTFSParser (future).
class IConnectionParser {
public:
  virtual ~IConnectionParser() = default;

  /// @brief Parses connection data from the given file.
  /// @param filename Path to the data file.
  /// @return Adjacency list: from_id → {to_id → weight_seconds}.
  /// @throws std::runtime_error if the file cannot be read or parsed.
  [[nodiscard]] virtual std::unordered_map<
      uint64_t, std::unordered_map<uint64_t, uint64_t>>
  parse(const std::string &filename) const = 0;
};

} // namespace city_mapper
