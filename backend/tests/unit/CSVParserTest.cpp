/// @file CSVParserTest.cpp
/// @brief Unit tests for CSV station and connection parsers.

#include <gtest/gtest.h>

#include "data/parsers/CSVConnectionParser.hpp"
#include "data/parsers/CSVStationParser.hpp"

using namespace city_mapper;

// The TEST_DATA_DIR macro is set by CMake to point to tests/fixtures/
static const std::string FIXTURES_DIR = TEST_DATA_DIR;

// ──────────────────────────────────────────────
// CSVStationParser tests
// ──────────────────────────────────────────────

TEST(CSVStationParserTest, ParsesSmallFile) {
    CSVStationParser parser;
    auto stations = parser.parse(FIXTURES_DIR + "/small_stations.csv");

    EXPECT_EQ(stations.size(), 10u);
}

TEST(CSVStationParserTest, StationFieldsCorrect) {
    CSVStationParser parser;
    auto stations = parser.parse(FIXTURES_DIR + "/small_stations.csv");

    ASSERT_TRUE(stations.count(1) > 0);
    const auto& s1 = stations[1];
    EXPECT_EQ(s1.id, 1u);
    EXPECT_EQ(s1.name, "A");
    EXPECT_EQ(s1.line_id, "Ligne de la station A");
    EXPECT_EQ(s1.address, "Adresse de la station A");
}

TEST(CSVStationParserTest, AllStationsHaveIds) {
    CSVStationParser parser;
    auto stations = parser.parse(FIXTURES_DIR + "/small_stations.csv");

    for (const auto& [id, station] : stations) {
        EXPECT_EQ(id, station.id) << "ID mismatch for station: " << station.name;
        EXPECT_FALSE(station.name.empty()) << "Empty name for station ID: " << id;
    }
}

TEST(CSVStationParserTest, ThrowsOnMissingFile) {
    CSVStationParser parser;
    EXPECT_THROW(parser.parse("nonexistent_file.csv"), std::runtime_error);
}

TEST(CSVStationParserTest, NoOffByOneEOFBug) {
    CSVStationParser parser;
    auto stations = parser.parse(FIXTURES_DIR + "/small_stations.csv");

    // The original code had an off-by-one bug that read an extra empty entry.
    // With 10 data lines, we should get exactly 10 stations.
    EXPECT_EQ(stations.size(), 10u);

    // No station with ID 0 (garbage from empty line)
    EXPECT_EQ(stations.count(0), 0u);
}

// ──────────────────────────────────────────────
// CSVConnectionParser tests
// ──────────────────────────────────────────────

TEST(CSVConnectionParserTest, ParsesSmallFile) {
    CSVConnectionParser parser;
    auto connections = parser.parse(FIXTURES_DIR + "/small_connections.csv");

    // Count total connections
    size_t total = 0;
    for (const auto& [from, neighbors] : connections) {
        total += neighbors.size();
    }

    EXPECT_EQ(total, 22u);
}

TEST(CSVConnectionParserTest, ConnectionWeightsCorrect) {
    CSVConnectionParser parser;
    auto connections = parser.parse(FIXTURES_DIR + "/small_connections.csv");

    // 1 -> 2 should be 85s
    ASSERT_TRUE(connections.count(1) > 0);
    ASSERT_TRUE(connections[1].count(2) > 0);
    EXPECT_EQ(connections[1][2], 85u);

    // 9 -> 10 should be 84s
    ASSERT_TRUE(connections.count(9) > 0);
    ASSERT_TRUE(connections[9].count(10) > 0);
    EXPECT_EQ(connections[9][10], 84u);
}

TEST(CSVConnectionParserTest, ThrowsOnMissingFile) {
    CSVConnectionParser parser;
    EXPECT_THROW(parser.parse("nonexistent_file.csv"), std::runtime_error);
}

TEST(CSVConnectionParserTest, NoOffByOneEOFBug) {
    CSVConnectionParser parser;
    auto connections = parser.parse(FIXTURES_DIR + "/small_connections.csv");

    // No connection from station 0 (garbage from EOF bug)
    EXPECT_EQ(connections.count(0), 0u);

    // No connection to station 0
    for (const auto& [from, neighbors] : connections) {
        EXPECT_EQ(neighbors.count(0), 0u)
            << "Found ghost connection from " << from << " to 0";
    }
}
