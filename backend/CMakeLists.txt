cmake_minimum_required(VERSION 3.20)
project(CityMapper VERSION 1.0.0 LANGUAGES CXX)

# ──────────────────────────────────────────────
# C++ Standard
# ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ──────────────────────────────────────────────
# Build type defaults
# ──────────────────────────────────────────────
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# ──────────────────────────────────────────────
# Conan integration (find packages installed by Conan)
# ──────────────────────────────────────────────
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

find_package(Drogon REQUIRED)
find_package(spdlog REQUIRED)

# ──────────────────────────────────────────────
# Core library (algorithms, graph, parsers, cache)
# ──────────────────────────────────────────────
add_library(city_mapper_core STATIC
    # Graph
    src/core/graph/Graph.cpp

    # Algorithms
    src/core/algorithms/Dijkstra.cpp
    src/core/algorithms/AStar.cpp

    # Parsers
    src/data/parsers/CSVStationParser.cpp
    src/data/parsers/CSVConnectionParser.cpp

    # Loaders
    src/data/loaders/GraphLoader.cpp
)

target_include_directories(city_mapper_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(city_mapper_core PUBLIC
    spdlog::spdlog
)

# ──────────────────────────────────────────────
# API Server (Drogon HTTP controllers)
# ──────────────────────────────────────────────
add_executable(city_mapper_server
    src/main.cpp
    src/api/controllers/HealthController.cpp
    src/api/controllers/StationController.cpp
    src/api/controllers/RouteController.cpp
    src/api/middleware/CORSFilter.cpp
)

target_include_directories(city_mapper_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(city_mapper_server PRIVATE
    city_mapper_core
    Drogon::Drogon
)

# ──────────────────────────────────────────────
# Tests
# ──────────────────────────────────────────────
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ──────────────────────────────────────────────
# Install rules
# ──────────────────────────────────────────────
install(TARGETS city_mapper_server DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/../data/ DESTINATION share/city-mapper/data)
