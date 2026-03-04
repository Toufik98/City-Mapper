/// @file AStarTest.cpp
/// @brief Unit tests for the A* algorithm with Haversine heuristic.

#include <gtest/gtest.h>

#include "core/algorithms/AStar.hpp"
#include "core/algorithms/Dijkstra.hpp"
#include "core/graph/Graph.hpp"

using namespace city_mapper;

class AStarTest : public ::testing::Test {
protected:
    Graph graph;
    AStar astar;
    Dijkstra dijkstra;

    void SetUp() override {
        // Same graph as DijkstraTest but with geographic coordinates
        // Paris area coordinates for A* heuristic testing
        struct StationData {
            uint64_t id;
            std::string name;
            double lat;
            double lon;
        };

        std::vector<StationData> stations = {
            {1, "Station A", 48.8566, 2.3522},   // Central Paris
            {2, "Station B", 48.8606, 2.3376},   // Near Louvre
            {3, "Station C", 48.8530, 2.3499},   // Near Notre-Dame
            {4, "Station D", 48.8738, 2.2950},   // Near Arc de Triomphe
            {5, "Station E", 48.8462, 2.3464},   // Near Luxembourg
            {6, "Station F", 48.8649, 2.3210},   // Near Opéra
            {7, "Station G", 48.8441, 2.3559},   // Near Gobelins
            {8, "Station H", 48.8396, 2.3276},   // Near Denfert
            {9, "Station I", 48.8676, 2.3049},   // Near Madeleine
            {10, "Station J", 48.8322, 2.3218},  // Near Cité U
        };

        for (const auto& s : stations) {
            Station station;
            station.id = s.id;
            station.name = s.name;
            station.line_id = "1";
            station.line_name = "Line 1";
            station.latitude = s.lat;
            station.longitude = s.lon;
            graph.add_station(s.id, station);
        }

        // Same edges as the test data
        graph.add_connection(9, 10, 84);
        graph.add_connection(10, 5, 493);
        graph.add_connection(10, 9, 84);
        graph.add_connection(8, 4, 183);
        graph.add_connection(3, 1, 217);
        graph.add_connection(8, 3, 91);
        graph.add_connection(7, 3, 186);
        graph.add_connection(3, 8, 103);
        graph.add_connection(6, 2, 80);
        graph.add_connection(9, 6, 250);
        graph.add_connection(8, 10, 167);
        graph.add_connection(2, 1, 85);
        graph.add_connection(2, 6, 80);
        graph.add_connection(6, 9, 278);
        graph.add_connection(3, 7, 186);
        graph.add_connection(5, 1, 173);
        graph.add_connection(4, 8, 183);
        graph.add_connection(1, 5, 173);
        graph.add_connection(1, 2, 85);
        graph.add_connection(10, 8, 167);
        graph.add_connection(1, 3, 217);
        graph.add_connection(5, 10, 502);
    }
};

TEST_F(AStarTest, SameResultAsDijkstra) {
    // A* with admissible heuristic must find the same optimal cost as Dijkstra
    for (uint64_t start = 1; start <= 10; ++start) {
        for (uint64_t end = 1; end <= 10; ++end) {
            auto astar_result = astar.compute(graph, start, end);
            auto dijkstra_result = dijkstra.compute(graph, start, end);

            EXPECT_EQ(astar_result.found, dijkstra_result.found)
                << "Mismatch for path " << start << " -> " << end;

            if (astar_result.found && dijkstra_result.found) {
                EXPECT_EQ(astar_result.total_duration_seconds,
                          dijkstra_result.total_duration_seconds)
                    << "Cost mismatch for path " << start << " -> " << end
                    << ": A*=" << astar_result.total_duration_seconds
                    << " Dijkstra=" << dijkstra_result.total_duration_seconds;
            }
        }
    }
}

TEST_F(AStarTest, SameStartAndEnd) {
    auto result = astar.compute(graph, 1, 1);
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.total_duration_seconds, 0u);
}

TEST_F(AStarTest, DirectConnection) {
    auto result = astar.compute(graph, 1, 2);
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.total_duration_seconds, 85u);
}

TEST_F(AStarTest, AlgorithmName) {
    EXPECT_EQ(astar.name(), "A*");
}

TEST_F(AStarTest, NonexistentStation) {
    auto result = astar.compute(graph, 1, 999);
    EXPECT_FALSE(result.found);
}

TEST_F(AStarTest, DisconnectedStation) {
    Station isolated{100, "Isolated", "99", "Line 99", "Nowhere", 49.0, 2.0};
    graph.add_station(100, isolated);

    auto result = astar.compute(graph, 1, 100);
    EXPECT_FALSE(result.found);
}

TEST_F(AStarTest, FallbackWithoutCoordinates) {
    // Build a graph without coordinates — A* should degrade to Dijkstra
    Graph no_coords_graph;

    for (uint64_t i = 1; i <= 3; ++i) {
        Station s;
        s.id = i;
        s.name = "S" + std::to_string(i);
        s.line_id = "1";
        // No lat/lon — defaults to 0.0
        no_coords_graph.add_station(i, s);
    }

    no_coords_graph.add_connection(1, 2, 100);
    no_coords_graph.add_connection(2, 3, 200);
    no_coords_graph.add_connection(1, 3, 500);

    auto result = astar.compute(no_coords_graph, 1, 3);
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.total_duration_seconds, 300u);  // 1->2->3 = 100+200
}
