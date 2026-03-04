/// @file HaversineTest.cpp
/// @brief Unit tests for Haversine distance and travel time estimation.

#include <gtest/gtest.h>

#include <cmath>

#include "core/utils/Haversine.hpp"
#include "core/utils/TimeFormatter.hpp"
#include "core/utils/StringUtils.hpp"

using namespace city_mapper;

// ──────────────────────────────────────────────
// Haversine distance tests
// ──────────────────────────────────────────────

TEST(HaversineTest, SamePoint) {
    double d = haversine_distance(48.8566, 2.3522, 48.8566, 2.3522);
    EXPECT_NEAR(d, 0.0, 0.001);
}

TEST(HaversineTest, KnownDistance_ParisToMarseille) {
    // Paris (48.8566, 2.3522) to Marseille (43.2965, 5.3698)
    // Known distance: ~660 km
    double d = haversine_distance(48.8566, 2.3522, 43.2965, 5.3698);
    EXPECT_NEAR(d, 660000.0, 20000.0);  // Within 20 km
}

TEST(HaversineTest, KnownDistance_ParisShortDistance) {
    // Châtelet (48.8584, 2.3478) to Gare de Lyon (48.8443, 2.3737)
    // Known distance: ~2.2 km
    double d = haversine_distance(48.8584, 2.3478, 48.8443, 2.3737);
    EXPECT_NEAR(d, 2200.0, 500.0);  // Within 500 m
}

TEST(HaversineTest, Symmetry) {
    double d1 = haversine_distance(48.8566, 2.3522, 43.2965, 5.3698);
    double d2 = haversine_distance(43.2965, 5.3698, 48.8566, 2.3522);
    EXPECT_NEAR(d1, d2, 0.001);
}

TEST(HaversineTest, EstimateTravelTimeZeroDistance) {
    EXPECT_EQ(estimate_travel_time_seconds(0.0), 0u);
}

TEST(HaversineTest, EstimateTravelTimeRealisticSpeed) {
    // 1000 meters at 40 km/h (11.11 m/s) ≈ 90 seconds
    uint64_t t = estimate_travel_time_seconds(1000.0);
    EXPECT_GT(t, 80u);
    EXPECT_LT(t, 100u);
}

// ──────────────────────────────────────────────
// TimeFormatter tests
// ──────────────────────────────────────────────

TEST(TimeFormatterTest, ZeroSeconds) {
    EXPECT_EQ(format_duration(0), "0s");
}

TEST(TimeFormatterTest, SecondsOnly) {
    EXPECT_EQ(format_duration(45), "45s");
}

TEST(TimeFormatterTest, MinutesAndSeconds) {
    EXPECT_EQ(format_duration(125), "2min 5s");
}

TEST(TimeFormatterTest, HoursMinutesSeconds) {
    EXPECT_EQ(format_duration(3661), "1h 1min 1s");
}

TEST(TimeFormatterTest, ExactMinutes) {
    EXPECT_EQ(format_duration(120), "2min");
}

TEST(TimeFormatterTest, CompactFormat) {
    EXPECT_EQ(format_duration_compact(30), "< 1 min");
    EXPECT_EQ(format_duration_compact(90), "2 min");    // Rounded
    EXPECT_EQ(format_duration_compact(1200), "20 min");
}

// ──────────────────────────────────────────────
// StringUtils tests
// ──────────────────────────────────────────────

TEST(StringUtilsTest, IEqualsBasic) {
    EXPECT_TRUE(iequals("Hello", "hello"));
    EXPECT_TRUE(iequals("ABC", "abc"));
    EXPECT_FALSE(iequals("ABC", "abcd"));
}

TEST(StringUtilsTest, IContains) {
    EXPECT_TRUE(icontains("Châtelet-Les Halles", "chatelet"));
    EXPECT_TRUE(icontains("Gare de Lyon", "GARE"));
    EXPECT_FALSE(icontains("Gare de Lyon", "Bercy"));
}

TEST(StringUtilsTest, TrimWhitespace) {
    std::string s = "  hello  ";
    trim(s);
    EXPECT_EQ(s, "hello");
}

TEST(StringUtilsTest, TrimEmpty) {
    std::string s = "";
    trim(s);
    EXPECT_EQ(s, "");
}

TEST(StringUtilsTest, ToLower) {
    EXPECT_EQ(to_lower("HELLO"), "hello");
    EXPECT_EQ(to_lower("Hello World"), "hello world");
}
