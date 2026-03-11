/// @file Graph.cpp
/// @brief Implementation of the Graph adjacency-list representation.

#include "Graph.hpp"

#include <algorithm>
#include <cctype>
#include <set>
#include <unordered_set>

#include "../utils/StringUtils.hpp"

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

/// @brief Diacritics-insensitive, case-insensitive search with deduplication.
/// @details Each unique station name appears only once in results, with all
///          its metro lines collected into the `lines` field.
std::vector<SearchResult> Graph::search_stations(const std::string& query, size_t limit) const {
    if (query.empty()) {
        return {};
    }

    // Normalize query: strip diacritics + lowercase
    const std::string norm_query = normalize_for_search(query);

    // Intermediate: scored match keyed by normalized station name
    struct ScoredMatch {
        std::string normalized_name;
        const Station* station;         // representative station
        std::set<std::string> line_ids; // ordered set of unique line IDs
        int score;                      // lower = better
    };

    // Map from normalized name → scored match
    std::unordered_map<std::string, ScoredMatch> seen;

    for (const auto& [id, station] : stations_) {
        std::string norm_name = normalize_for_search(station.name);

        // Check for substring match
        auto pos = norm_name.find(norm_query);
        if (pos == std::string::npos) {
            continue;
        }

        // Compute relevance score
        int score = 0;
        if (norm_name == norm_query) {
            // Exact match → highest priority
            score = 0;
        } else if (pos == 0) {
            // Prefix match
            score = 1;
        } else {
            // Substring match → lower priority, ordered by position
            score = static_cast<int>(pos) + 2;
        }
        // Favor shorter names (more specific matches)
        score += static_cast<int>(station.name.size()) / 10;

        auto it = seen.find(norm_name);
        if (it != seen.end()) {
            // Already seen this station name — just add the line ID
            it->second.line_ids.insert(station.line_id);
            // Keep best score
            if (score < it->second.score) {
                it->second.score = score;
                it->second.station = &station;
            }
        } else {
            ScoredMatch match;
            match.normalized_name = norm_name;
            match.station = &station;
            match.line_ids.insert(station.line_id);
            match.score = score;
            seen.emplace(norm_name, std::move(match));
        }
    }

    // Collect and sort by score
    std::vector<ScoredMatch*> sorted;
    sorted.reserve(seen.size());
    for (auto& [key, match] : seen) {
        sorted.push_back(&match);
    }
    std::sort(sorted.begin(), sorted.end(),
              [](const ScoredMatch* a, const ScoredMatch* b) {
                  if (a->score != b->score) return a->score < b->score;
                  return a->station->name < b->station->name;
              });

    // Build results up to limit
    std::vector<SearchResult> results;
    size_t count = std::min(limit, sorted.size());
    results.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        SearchResult r;
        r.station = *sorted[i]->station;
        r.lines.assign(sorted[i]->line_ids.begin(), sorted[i]->line_ids.end());
        results.push_back(std::move(r));
    }

    return results;
}

}  // namespace city_mapper

