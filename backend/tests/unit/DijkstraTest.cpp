/// @file DijkstraTest.cpp
/// @brief Unit tests for the optimized Dijkstra algorithm.

#include <gtest/gtest.h>

#include "core/algorithms/Dijkstra.hpp"
#include "core/graph/Graph.hpp"

using namespace city_mapper;

class DijkstraTest : public ::testing::Test {
protected:
    Graph graph;
    Dijkstra dijkstra;

    void SetUp() override {
        // Build the test graph from small_connections.csv:
        //
        //   Graph topology (10 nodes, 22 edges):
        //   1 <-> 2 (85s), 1 <-> 3 (217s), 1 <-> 5 (173s)
        //   2 <-> 6 (80s)
        //   3 <-> 7 (186s), 3 <-> 8 (91/103s)
        //   4 <-> 8 (183s)
        //   5 <-> 10 (493/502s)
        //   6 <-> 9 (250/278s)
        //   8 <-> 10 (167s)
        //   9 <-> 10 (84s)

        for (uint64_t i = 1; i <= 10; ++i) {
            Station s;
            s.id = i;
            s.name = "Station " + std::string(1, static_cast<char>('A' + i - 1));
            s.line_id = "1";
            s.line_name = "Line 1";
            graph.add_station(i, s);
        }

        // Edges from the test data
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

TEST_F(DijkstraTest, SameStartAndEnd) {
    auto result = dijkstra.compute(graph, 1, 1);
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.total_duration_seconds, 0u);
    EXPECT_EQ(result.raw_path.size(), 1u);
}

TEST_F(DijkstraTest, DirectConnection) {
    // 1 -> 2 should be 85s (direct edge)
    auto result = dijkstra.compute(graph, 1, 2);
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.total_duration_seconds, 85u);
    ASSERT_EQ(result.raw_path.size(), 2u);
    EXPECT_EQ(result.raw_path[0].first, 1u);
    EXPECT_EQ(result.raw_path[1].first, 2u);
}

TEST_F(DijkstraTest, TwoHopPath) {
    // 1 -> 2 -> 6 should be 85 + 80 = 165s
    auto result = dijkstra.compute(graph, 1, 6);
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.total_duration_seconds, 165u);
    ASSERT_EQ(result.raw_path.size(), 3u);
    EXPECT_EQ(result.raw_path[0].first, 1u);
    EXPECT_EQ(result.raw_path[1].first, 2u);
    EXPECT_EQ(result.raw_path[2].first, 6u);
}

TEST_F(DijkstraTest, ShortestPathNotDirect) {
    // 1 -> 3 direct = 217s, but via 1->3 = 217s is actually the shortest
    // (no shorter alternative in this graph)
    auto result = dijkstra.compute(graph, 1, 3);
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.total_duration_seconds, 217u);
}

TEST_F(DijkstraTest, MultiHopPath) {
    // 1 -> 8: via 1->3->8 = 217 + 103 = 320s
    auto result = dijkstra.compute(graph, 1, 8);
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.total_duration_seconds, 320u);
}

TEST_F(DijkstraTest, LongerPath) {
    // 1 -> 10: via 1->3->8->10 = 217+103+167 = 487s
    //    or via 1->5->10 = 173+502 = 675s
    // Shortest should be 487s
    auto result = dijkstra.compute(graph, 1, 10);
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.total_duration_seconds, 487u);
}

TEST_F(DijkstraTest, ReverseDirection) {
    // 10 -> 1: via 10->5->1 = 493+173 = 666s
    //    or via 10->8->3->1 = 167+91+217 = 475s
    //    or via 10->9->6->2->1 = 84+250+80+85 = 499s
    // Shortest should be 475s
    auto result = dijkstra.compute(graph, 10, 1);
    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.total_duration_seconds, 475u);
}

TEST_F(DijkstraTest, NonexistentStation) {
    auto result = dijkstra.compute(graph, 1, 999);
    EXPECT_FALSE(result.found);
}

TEST_F(DijkstraTest, NonexistentStartStation) {
    auto result = dijkstra.compute(graph, 999, 1);
    EXPECT_FALSE(result.found);
}

TEST_F(DijkstraTest, AlgorithmName) {
    EXPECT_EQ(dijkstra.name(), "Dijkstra");
}

TEST_F(DijkstraTest, PathContainsSegments) {
    auto result = dijkstra.compute(graph, 1, 6);
    ASSERT_TRUE(result.found);
    EXPECT_GE(result.segments.size(), 1u);
}

TEST_F(DijkstraTest, DisconnectedGraph) {
    // Add an isolated station with no connections
    Station isolated{100, "Isolated", "99", "Line 99", "Nowhere"};
    graph.add_station(100, isolated);

    auto result = dijkstra.compute(graph, 1, 100);
    EXPECT_FALSE(result.found);
}
