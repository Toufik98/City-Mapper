/// @file AStar.cpp
/// @brief A* algorithm implementation with Haversine heuristic.

#include "AStar.hpp"

#include <functional>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../utils/Haversine.hpp"

namespace city_mapper {

RouteResult AStar::compute(const Graph& graph, uint64_t start, uint64_t end) {
    RouteResult result;
    result.algorithm = name();
    result.found = false;

    // Trivial case
    if (start == end) {
        result.found = true;
        result.total_duration_seconds = 0;
        result.total_transfers = 0;
        result.total_stations = 1;
        result.raw_path.emplace_back(start, 0);
        return result;
    }

    if (!graph.has_station(start) || !graph.has_station(end)) {
        return result;
    }

    // Get destination station for heuristic computation
    auto end_station = graph.get_station(end);
    const double end_lat = end_station->latitude;
    const double end_lon = end_station->longitude;
    const bool has_coords = end_station->has_coordinates();

    // Heuristic function: estimated time from a station to the destination
    auto heuristic = [&](uint64_t station_id) -> uint64_t {
        if (!has_coords) {
            return 0;  // Fallback: degrade to Dijkstra if no coordinates
        }
        auto station = graph.get_station(station_id);
        if (!station || !station->has_coordinates()) {
            return 0;
        }
        double distance = haversine_distance(
            station->latitude, station->longitude, end_lat, end_lon);
        return estimate_travel_time_seconds(distance);
    };

    // Min-heap: (f_score, station_id) where f_score = g_score + h(n)
    using PQEntry = std::pair<uint64_t, uint64_t>;
    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> open;

    // g_score: actual cost from start to each node
    std::unordered_map<uint64_t, uint64_t> g_score;

    // Parent map for path reconstruction
    std::unordered_map<uint64_t, uint64_t> parent;

    // Closed set: fully processed nodes
    std::unordered_set<uint64_t> closed;

    // Initialize
    g_score[start] = 0;
    open.push({heuristic(start), start});

    while (!open.empty()) {
        auto [f_cost, curr_id] = open.top();
        open.pop();

        // Skip already-processed nodes (stale PQ entries)
        if (closed.count(curr_id)) {
            continue;
        }
        closed.insert(curr_id);

        // Goal reached
        if (curr_id == end) {
            result.found = true;
            result.total_duration_seconds = g_score[curr_id];
            break;
        }

        uint64_t curr_g = g_score[curr_id];

        // Expand neighbors
        for (const auto& edge : graph.get_neighbors(curr_id)) {
            if (closed.count(edge.target_id)) {
                continue;
            }

            uint64_t tentative_g = curr_g + edge.weight_seconds;
            auto it = g_score.find(edge.target_id);

            if (it == g_score.end() || tentative_g < it->second) {
                g_score[edge.target_id] = tentative_g;
                parent[edge.target_id] = curr_id;

                uint64_t f = tentative_g + heuristic(edge.target_id);
                open.push({f, edge.target_id});
            }
        }
    }

    if (!result.found) {
        return result;
    }

    // Reconstruct path
    std::vector<uint64_t> path_ids;
    uint64_t current = end;
    while (current != start) {
        path_ids.push_back(current);
        current = parent[current];
    }
    path_ids.push_back(start);
    std::reverse(path_ids.begin(), path_ids.end());

    // Build raw_path
    result.raw_path.reserve(path_ids.size());
    for (uint64_t id : path_ids) {
        result.raw_path.emplace_back(id, g_score[id]);
    }

    // Build detailed segments
    result.total_stations = static_cast<uint32_t>(path_ids.size());
    result.total_transfers = 0;

    std::string current_line;

    for (size_t i = 0; i + 1 < path_ids.size(); ++i) {
        uint64_t from_id = path_ids[i];
        uint64_t to_id = path_ids[i + 1];

        auto from_station = graph.get_station(from_id);
        auto to_station = graph.get_station(to_id);

        if (!from_station || !to_station) continue;

        uint64_t seg_duration = g_score[to_id] - g_score[from_id];

        ConnectionType seg_type = ConnectionType::RIDE;
        for (const auto& edge : graph.get_neighbors(from_id)) {
            if (edge.target_id == to_id) {
                seg_type = edge.type;
                break;
            }
        }

        if (!current_line.empty() && from_station->line_id != current_line) {
            ++result.total_transfers;
        }
        current_line = from_station->line_id;

        RouteSegment segment;
        segment.from_station = *from_station;
        segment.to_station = *to_station;
        segment.duration_seconds = seg_duration;
        segment.type = seg_type;
        segment.line_id = from_station->line_id;
        segment.line_name = from_station->line_name;

        result.segments.push_back(std::move(segment));
    }

    return result;
}

}  // namespace city_mapper
