/// @file Graph.cpp
/// @brief Implementation of the Graph adjacency-list representation.

#include "Graph.hpp"

#include <algorithm>
#include <cctype>

namespace city_mapper {

const std::vector<WeightedEdge> Graph::EMPTY_NEIGHBORS{};

void Graph::add_station(uint64_t id, Station station) {
    station.id = id;
    stations_[id] = std::move(station);
}

void Graph::add_connection(uint64_t from_id, uint64_t to_id, uint64_t weight,
                           ConnectionType type) {
    adjacency_[from_id].push_back(WeightedEdge{to_id, weight, type});
    ++total_connections_;
}

std::optional<Station> Graph::get_station(uint64_t id) const {
    auto it = stations_.find(id);
    if (it != stations_.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::unordered_map<uint64_t, Station>& Graph::get_stations() const noexcept {
    return stations_;
}

const std::vector<WeightedEdge>& Graph::get_neighbors(uint64_t station_id) const {
    auto it = adjacency_.find(station_id);
    if (it != adjacency_.end()) {
        return it->second;
    }
    return EMPTY_NEIGHBORS;
}

bool Graph::has_station(uint64_t id) const noexcept {
    return stations_.count(id) > 0;
}

size_t Graph::station_count() const noexcept {
    return stations_.size();
}

size_t Graph::connection_count() const noexcept {
    return total_connections_;
}

const std::unordered_map<uint64_t, std::vector<WeightedEdge>>&
Graph::get_adjacency() const noexcept {
    return adjacency_;
}

/// @brief Case-insensitive substring search with prefix-match priority.
/// @complexity O(n * m) where n = stations, m = max(query.length(), name.length())
std::vector<Station> Graph::search_stations(const std::string& query, size_t limit) const {
    if (query.empty()) {
        return {};
    }

    // Convert query to lowercase once
    std::string lower_query;
    lower_query.reserve(query.size());
    for (char c : query) {
        lower_query.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    struct ScoredStation {
        const Station* station;
        int score;  // lower = better match
    };

    std::vector<ScoredStation> matches;
    matches.reserve(64);  // Reasonable pre-allocation

    for (const auto& [id, station] : stations_) {
        // Convert station name to lowercase
        std::string lower_name;
        lower_name.reserve(station.name.size());
        for (char c : station.name) {
            lower_name.push_back(
                static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }

        // Check for substring match
        auto pos = lower_name.find(lower_query);
        if (pos != std::string::npos) {
            int score = 0;
            if (pos == 0) {
                // Exact prefix match → highest priority
                score = 0;
            } else {
                // Substring match → lower priority, ordered by position
                score = static_cast<int>(pos) + 1;
            }
            // Favor shorter names (more specific matches)
            score += static_cast<int>(station.name.size()) / 10;

            matches.push_back({&station, score});
        }
    }

    // Sort by score ascending (best matches first)
    std::sort(matches.begin(), matches.end(),
              [](const ScoredStation& a, const ScoredStation& b) {
                  return a.score < b.score;
              });

    // Collect results up to limit
    std::vector<Station> results;
    size_t count = std::min(limit, matches.size());
    results.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        results.push_back(*matches[i].station);
    }

    return results;
}

}  // namespace city_mapper
