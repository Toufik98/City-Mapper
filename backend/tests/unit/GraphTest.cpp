/// @file GraphTest.cpp
/// @brief Unit tests for the Graph class.

#include <gtest/gtest.h>

#include "core/graph/Graph.hpp"

using namespace city_mapper;

class GraphTest : public ::testing::Test {
protected:
    Graph graph;

    void SetUp() override {
        // Build a small test graph:
        //   1 --85--> 2 --80--> 6
        //   1 --217-> 3
        //   3 --103-> 8
        Station s1{1, "Station A", "1", "Line 1", "Address A", 48.85, 2.35};
        Station s2{2, "Station B", "1", "Line 1", "Address B", 48.86, 2.36};
        Station s3{3, "Station C", "2", "Line 2", "Address C", 48.87, 2.37};
        Station s6{6, "Station F", "1", "Line 1", "Address F", 48.88, 2.38};
        Station s8{8, "Station H", "2", "Line 2", "Address H", 48.84, 2.34};

        graph.add_station(1, s1);
        graph.add_station(2, s2);
        graph.add_station(3, s3);
        graph.add_station(6, s6);
        graph.add_station(8, s8);

        graph.add_connection(1, 2, 85, ConnectionType::RIDE);
        graph.add_connection(2, 6, 80, ConnectionType::RIDE);
        graph.add_connection(1, 3, 217, ConnectionType::RIDE);
        graph.add_connection(3, 8, 103, ConnectionType::RIDE);
    }
};

TEST_F(GraphTest, StationCount) {
    EXPECT_EQ(graph.station_count(), 5u);
}

TEST_F(GraphTest, ConnectionCount) {
    EXPECT_EQ(graph.connection_count(), 4u);
}

TEST_F(GraphTest, HasStation) {
    EXPECT_TRUE(graph.has_station(1));
    EXPECT_TRUE(graph.has_station(2));
    EXPECT_FALSE(graph.has_station(99));
}

TEST_F(GraphTest, GetStationFound) {
    auto station = graph.get_station(1);
    ASSERT_TRUE(station.has_value());
    EXPECT_EQ(station->name, "Station A");
    EXPECT_EQ(station->line_id, "1");
}

TEST_F(GraphTest, GetStationNotFound) {
    auto station = graph.get_station(999);
    EXPECT_FALSE(station.has_value());
}

TEST_F(GraphTest, GetNeighbors) {
    const auto& neighbors = graph.get_neighbors(1);
    EXPECT_EQ(neighbors.size(), 2u);

    // Check that neighbors include stations 2 and 3
    bool has_2 = false, has_3 = false;
    for (const auto& edge : neighbors) {
        if (edge.target_id == 2) {
            has_2 = true;
            EXPECT_EQ(edge.weight_seconds, 85u);
        }
        if (edge.target_id == 3) {
            has_3 = true;
            EXPECT_EQ(edge.weight_seconds, 217u);
        }
    }
    EXPECT_TRUE(has_2);
    EXPECT_TRUE(has_3);
}

TEST_F(GraphTest, GetNeighborsEmpty) {
    // Station 8 has no outgoing connections in this test graph
    const auto& neighbors = graph.get_neighbors(8);
    EXPECT_TRUE(neighbors.empty());
}

TEST_F(GraphTest, GetNeighborsNonexistentStation) {
    const auto& neighbors = graph.get_neighbors(999);
    EXPECT_TRUE(neighbors.empty());
}

TEST_F(GraphTest, SearchStationsByName) {
    auto results = graph.search_stations("Station", 10);
    EXPECT_EQ(results.size(), 5u);
}

TEST_F(GraphTest, SearchStationsByPrefix) {
    auto results = graph.search_stations("Station A", 10);
    ASSERT_GE(results.size(), 1u);
    EXPECT_EQ(results[0].name, "Station A");
}

TEST_F(GraphTest, SearchStationsCaseInsensitive) {
    auto results = graph.search_stations("station a", 10);
    ASSERT_GE(results.size(), 1u);
    EXPECT_EQ(results[0].name, "Station A");
}

TEST_F(GraphTest, SearchStationsLimit) {
    auto results = graph.search_stations("Station", 2);
    EXPECT_EQ(results.size(), 2u);
}

TEST_F(GraphTest, SearchStationsNoMatch) {
    auto results = graph.search_stations("Nonexistent", 10);
    EXPECT_TRUE(results.empty());
}

TEST_F(GraphTest, SearchStationsEmptyQuery) {
    auto results = graph.search_stations("", 10);
    EXPECT_TRUE(results.empty());
}

TEST_F(GraphTest, AddStationOverwrite) {
    Station new_s1{1, "Station A Updated", "1", "Line 1", "New Address", 0.0, 0.0};
    graph.add_station(1, new_s1);

    EXPECT_EQ(graph.station_count(), 5u);  // No new station added
    auto station = graph.get_station(1);
    ASSERT_TRUE(station.has_value());
    EXPECT_EQ(station->name, "Station A Updated");
}
