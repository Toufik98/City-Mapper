#pragma once

/// @file CSVConnectionParser.hpp
/// @brief Parses connection data from the RATP CSV format.

#include "IConnectionParser.hpp"

namespace city_mapper {

/// @brief Parses connections from CSV files in the RATP format.
/// @details Expected CSV format (with header):
///   `from_stop_id,to_stop_id,min_transfer_time`
///
/// Fixes the off-by-one EOF bug from the original `Station_parser::read_connections`.
/// The original code used `!file.eof()` which also causes the last line to be read twice
/// or an empty line to be parsed.
class CSVConnectionParser : public IConnectionParser {
public:
    /// @brief Parses connection CSV file into an adjacency list.
    /// @param filename Path to the CSV file.
    /// @return Adjacency list: from_id → {to_id → weight_seconds}.
    /// @throws std::runtime_error if file cannot be opened.
    /// @complexity O(n) where n = number of lines in the file.
    [[nodiscard]] std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>>
    parse(const std::string& filename) const override;
};

}  // namespace city_mapper
