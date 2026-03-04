#pragma once

/// @file RouteCache.hpp
/// @brief Route-specific LRU cache wrapper.
/// @details Caches computed routes keyed by (start_id, end_id, algorithm).

#include <cstdint>
#include <functional>
#include <string>

#include "LRUCache.hpp"
#include "../graph/RouteResult.hpp"

namespace city_mapper {

/// @brief Cache key for route lookups.
struct RouteCacheKey {
    uint64_t start_id;
    uint64_t end_id;
    std::string algorithm;

    bool operator==(const RouteCacheKey& other) const noexcept {
        return start_id == other.start_id &&
               end_id == other.end_id &&
               algorithm == other.algorithm;
    }
};

}  // namespace city_mapper

// Hash specialization for RouteCacheKey
namespace std {
template <>
struct hash<city_mapper::RouteCacheKey> {
    size_t operator()(const city_mapper::RouteCacheKey& k) const noexcept {
        size_t h = hash<uint64_t>{}(k.start_id);
        h ^= hash<uint64_t>{}(k.end_id) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= hash<string>{}(k.algorithm) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};
}  // namespace std

namespace city_mapper {

/// @brief LRU cache specialized for route computation results.
/// @details Default capacity: 10,000 entries.
/// Thread-safe via the underlying LRUCache implementation.
class RouteCache {
public:
    /// @brief Constructs a route cache with the given capacity.
    /// @param capacity Maximum number of cached routes (default: 10,000).
    explicit RouteCache(size_t capacity = 10'000) : cache_(capacity) {}

    /// @brief Looks up a cached route.
    /// @param start_id Source station ID.
    /// @param end_id Destination station ID.
    /// @param algorithm Algorithm name.
    /// @return Cached RouteResult if found, std::nullopt otherwise.
    std::optional<RouteResult> get(uint64_t start_id, uint64_t end_id,
                                   const std::string& algorithm) {
        return cache_.get({start_id, end_id, algorithm});
    }

    /// @brief Caches a computed route.
    /// @param start_id Source station ID.
    /// @param end_id Destination station ID.
    /// @param algorithm Algorithm name.
    /// @param result The computed route to cache.
    void put(uint64_t start_id, uint64_t end_id,
             const std::string& algorithm, RouteResult result) {
        cache_.put({start_id, end_id, algorithm}, std::move(result));
    }

    /// @brief Returns cache statistics for monitoring.
    [[nodiscard]] size_t size() const { return cache_.size(); }
    [[nodiscard]] size_t capacity() const { return cache_.capacity(); }
    [[nodiscard]] double hit_ratio() const { return cache_.hit_ratio(); }
    [[nodiscard]] uint64_t hit_count() const { return cache_.hit_count(); }
    [[nodiscard]] uint64_t miss_count() const { return cache_.miss_count(); }

    /// @brief Clears all cached routes.
    void clear() { cache_.clear(); }

private:
    LRUCache<RouteCacheKey, RouteResult> cache_;
};

}  // namespace city_mapper
