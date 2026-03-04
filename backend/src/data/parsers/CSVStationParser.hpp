#pragma once

/// @file CSVStationParser.hpp
/// @brief Parses station data from the RATP CSV format.

#include "IStationParser.hpp"

namespace city_mapper {

/// @brief Parses stations from CSV files in the RATP format.
/// @details Expected CSV format (with header):
///   `name,id,line_id,address,line_name`
///
/// Fixes the off-by-one EOF bug from the original `Station_parser::read_stations`.
/// The original code used `file.good()` which reads one extra empty record at EOF.
class CSVStationParser : public IStationParser {
public:
    /// @brief Parses station CSV file into a station map.
    /// @param filename Path to the CSV file.
    /// @return Map of station_id → Station.
    /// @throws std::runtime_error if file cannot be opened.
    /// @complexity O(n) where n = number of lines in the file.
    [[nodiscard]] std::unordered_map<uint64_t, Station> parse(
        const std::string& filename) const override;
};

}  // namespace city_mapper
