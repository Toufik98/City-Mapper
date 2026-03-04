/// @file CSVStationParser.cpp
/// @brief Implementation of CSV station parser with EOF bug fix.

#include "CSVStationParser.hpp"

#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>

namespace city_mapper {

std::unordered_map<uint64_t, Station> CSVStationParser::parse(
    const std::string& filename) const {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("CSVStationParser: Cannot open file: " + filename);
    }

    std::unordered_map<uint64_t, Station> stations;
    std::string line;

    // Skip header line
    // Format: string_name_station,uint32_s_id,string_short_line,string_adress_station,string_desc_line
    std::getline(file, line);

    // Parse each station line
    // FIX: Use std::getline return value to detect EOF correctly.
    // The original code used `file.good()` which caused an off-by-one error
    // (reading one extra empty record when the file ends with a newline).
    std::string name, id_str, line_id, address, line_name;

    while (std::getline(file, name, ',')) {
        // Guard against empty trailing line
        if (name.empty() && file.peek() == EOF) {
            break;
        }

        std::getline(file, id_str, ',');
        std::getline(file, line_id, ',');
        std::getline(file, address, ',');
        std::getline(file, line_name);

        // Trim trailing whitespace/carriage return from line_name
        while (!line_name.empty() &&
               (line_name.back() == '\r' || line_name.back() == '\n' ||
                line_name.back() == ' ')) {
            line_name.pop_back();
        }

        uint64_t id = std::strtoull(id_str.c_str(), nullptr, 10);

        Station station;
        station.id = id;
        station.name = std::move(name);
        station.line_id = std::move(line_id);
        station.address = std::move(address);
        station.line_name = std::move(line_name);
        station.latitude = 0.0;   // Will be populated from GTFS data
        station.longitude = 0.0;

        stations[id] = std::move(station);

        // Reset for next iteration
        name.clear();
        id_str.clear();
        line_id.clear();
        address.clear();
        line_name.clear();
    }

    return stations;
}

}  // namespace city_mapper
