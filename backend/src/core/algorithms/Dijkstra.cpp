/// @file Dijkstra.cpp
/// @brief Optimized Dijkstra's algorithm implementation.

#include "Dijkstra.hpp"

#include <functional>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace city_mapper {

RouteResult Dijkstra::compute(const Graph& graph, uint64_t start, uint64_t end) {
    RouteResult result;
    result.algorithm = name();
    result.found = false;

    // Trivial case: start == end
    if (start == end) {
        result.found = true;
        result.total_duration_seconds = 0;
        result.total_transfers = 0;
        result.total_stations = 1;
        result.raw_path.emplace_back(start, 0);
        return result;
    }

    // Validate that both stations exist
    if (!graph.has_station(start) || !graph.has_station(end)) {
        return result;
    }

    // Min-heap: (cost, station_id) — std::greater yields smallest cost first
    using PQEntry = std::pair<uint64_t, uint64_t>;
    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> pq;

    // Distance map: station_id → best known cost from start
    std::unordered_map<uint64_t, uint64_t> dist;

    // Parent map: station_id → predecessor station_id (for path reconstruction)
    std::unordered_map<uint64_t, uint64_t> parent;

    // Visited set: O(1) lookup instead of the original O(V) linear scan
    std::unordered_set<uint64_t> visited;

    // Initialize source
    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [curr_cost, curr_id] = pq.top();
        pq.pop();

        // Skip if already visited (handles stale entries in the PQ)
        if (visited.count(curr_id)) {
            continue;
        }
        visited.insert(curr_id);

        // Destination reached — shortest path found
        if (curr_id == end) {
            result.found = true;
            result.total_duration_seconds = curr_cost;
            break;
        }

        // Relax all neighbors
        for (const auto& edge : graph.get_neighbors(curr_id)) {
            if (visited.count(edge.target_id)) {
                continue;
            }

            uint64_t new_cost = curr_cost + edge.weight_seconds;
            auto it = dist.find(edge.target_id);

            if (it == dist.end() || new_cost < it->second) {
                dist[edge.target_id] = new_cost;
                parent[edge.target_id] = curr_id;
                pq.push({new_cost, edge.target_id});
            }
        }
    }

    if (!result.found) {
        return result;
    }

    // Reconstruct path: walk parent pointers from end to start
    std::vector<uint64_t> path_ids;
    uint64_t current = end;
    while (current != start) {
        path_ids.push_back(current);
        current = parent[current];
    }
    path_ids.push_back(start);

    // Reverse to get start → end order
    std::reverse(path_ids.begin(), path_ids.end());

    // Build raw_path with cumulative costs
    result.raw_path.reserve(path_ids.size());
    for (uint64_t id : path_ids) {
        result.raw_path.emplace_back(id, dist[id]);
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

        if (!from_station || !to_station) {
            continue;
        }

        // Calculate segment duration
        uint64_t seg_duration = dist[to_id] - dist[from_id];

        // Determine connection type
        ConnectionType seg_type = ConnectionType::RIDE;
        for (const auto& edge : graph.get_neighbors(from_id)) {
            if (edge.target_id == to_id) {
                seg_type = edge.type;
                break;
            }
        }

        // Track line changes for transfer count
        if (from_station->line_id != current_line && !current_line.empty()) {
            if (from_station->line_id != current_line) {
                ++result.total_transfers;
            }
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
