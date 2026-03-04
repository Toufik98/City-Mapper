/// @file CSVConnectionParser.cpp
/// @brief Implementation of CSV connection parser with EOF bug fix.

#include "CSVConnectionParser.hpp"

#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>

namespace city_mapper {

std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>>
CSVConnectionParser::parse(const std::string& filename) const {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("CSVConnectionParser: Cannot open file: " + filename);
    }

    std::unordered_map<uint64_t, std::unordered_map<uint64_t, uint64_t>> connections;
    std::string line;

    // Skip header line
    // Format: uint32_from_stop_id,uint32_to_stop_id,uint32_min_transfer_time
    std::getline(file, line);

    // Parse each connection line
    // FIX: Use std::getline return value instead of file.eof() to avoid
    // reading past the end of file and creating ghost connections.
    std::string from_str, to_str, time_str;

    while (std::getline(file, line)) {
        // Skip empty lines (handles trailing newline at EOF)
        if (line.empty() || line == "\r") {
            continue;
        }

        // Trim trailing carriage return (Windows line endings)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // Parse the three comma-separated fields
        size_t first_comma = line.find(',');
        size_t second_comma = line.find(',', first_comma + 1);

        if (first_comma == std::string::npos || second_comma == std::string::npos) {
            continue;  // Malformed line — skip silently
        }

        from_str = line.substr(0, first_comma);
        to_str = line.substr(first_comma + 1, second_comma - first_comma - 1);
        time_str = line.substr(second_comma + 1);

        uint64_t from_id = std::strtoull(from_str.c_str(), nullptr, 10);
        uint64_t to_id = std::strtoull(to_str.c_str(), nullptr, 10);
        uint64_t weight = std::strtoull(time_str.c_str(), nullptr, 10);

        connections[from_id][to_id] = weight;
    }

    return connections;
}

}  // namespace city_mapper
