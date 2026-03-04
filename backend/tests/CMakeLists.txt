cmake_minimum_required(VERSION 3.20)

find_package(GTest REQUIRED)

# ──────────────────────────────────────────────
# Test fixture data
# ──────────────────────────────────────────────
set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fixtures")
add_compile_definitions(TEST_DATA_DIR="${TEST_DATA_DIR}")

# ──────────────────────────────────────────────
# Unit tests
# ──────────────────────────────────────────────
add_executable(city_mapper_tests
    unit/GraphTest.cpp
    unit/DijkstraTest.cpp
    unit/AStarTest.cpp
    unit/LRUCacheTest.cpp
    unit/CSVParserTest.cpp
    unit/HaversineTest.cpp
)

target_include_directories(city_mapper_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(city_mapper_tests PRIVATE
    city_mapper_core
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(city_mapper_tests)
