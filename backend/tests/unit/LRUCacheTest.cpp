/// @file LRUCacheTest.cpp
/// @brief Unit tests for the generic LRU cache and RouteCache.

#include <gtest/gtest.h>

#include <string>
#include <thread>
#include <vector>

#include "core/cache/LRUCache.hpp"
#include "core/cache/RouteCache.hpp"

using namespace city_mapper;

// ──────────────────────────────────────────────
// LRUCache<int, string> tests
// ──────────────────────────────────────────────

class LRUCacheTest : public ::testing::Test {
protected:
    LRUCache<int, std::string> cache{3};  // Capacity = 3
};

TEST_F(LRUCacheTest, PutAndGet) {
    cache.put(1, "one");
    auto val = cache.get(1);
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, "one");
}

TEST_F(LRUCacheTest, GetMissing) {
    auto val = cache.get(42);
    EXPECT_FALSE(val.has_value());
}

TEST_F(LRUCacheTest, EvictsLRU) {
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    // Cache is full (3/3). Insert 4 → should evict 1 (least recently used)
    cache.put(4, "four");

    EXPECT_FALSE(cache.get(1).has_value());  // Evicted
    EXPECT_TRUE(cache.get(2).has_value());
    EXPECT_TRUE(cache.get(3).has_value());
    EXPECT_TRUE(cache.get(4).has_value());
}

TEST_F(LRUCacheTest, AccessPromotesEntry) {
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    // Access 1 → promotes it to most recently used
    cache.get(1);

    // Insert 4 → should evict 2 (now LRU, not 1)
    cache.put(4, "four");

    EXPECT_TRUE(cache.get(1).has_value());   // Promoted, not evicted
    EXPECT_FALSE(cache.get(2).has_value());  // Evicted
    EXPECT_TRUE(cache.get(3).has_value());
    EXPECT_TRUE(cache.get(4).has_value());
}

TEST_F(LRUCacheTest, UpdateExistingKey) {
    cache.put(1, "one");
    cache.put(1, "ONE");

    auto val = cache.get(1);
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, "ONE");
    EXPECT_EQ(cache.size(), 1u);  // No duplicate
}

TEST_F(LRUCacheTest, Size) {
    EXPECT_EQ(cache.size(), 0u);
    cache.put(1, "one");
    EXPECT_EQ(cache.size(), 1u);
    cache.put(2, "two");
    EXPECT_EQ(cache.size(), 2u);
}

TEST_F(LRUCacheTest, Capacity) {
    EXPECT_EQ(cache.capacity(), 3u);
}

TEST_F(LRUCacheTest, Clear) {
    cache.put(1, "one");
    cache.put(2, "two");
    cache.clear();
    EXPECT_EQ(cache.size(), 0u);
    EXPECT_FALSE(cache.get(1).has_value());
}

TEST_F(LRUCacheTest, Contains) {
    cache.put(1, "one");
    EXPECT_TRUE(cache.contains(1));
    EXPECT_FALSE(cache.contains(2));
}

TEST_F(LRUCacheTest, HitMissCounters) {
    cache.put(1, "one");
    cache.get(1);       // Hit
    cache.get(2);       // Miss
    cache.get(1);       // Hit
    cache.get(3);       // Miss

    EXPECT_EQ(cache.hit_count(), 2u);
    EXPECT_EQ(cache.miss_count(), 2u);
    EXPECT_DOUBLE_EQ(cache.hit_ratio(), 0.5);
}

TEST_F(LRUCacheTest, ZeroCapacityDefaultsToOne) {
    LRUCache<int, std::string> tiny_cache(0);
    EXPECT_EQ(tiny_cache.capacity(), 1u);
    tiny_cache.put(1, "one");
    tiny_cache.put(2, "two");
    EXPECT_EQ(tiny_cache.size(), 1u);
    EXPECT_FALSE(tiny_cache.get(1).has_value());
    EXPECT_TRUE(tiny_cache.get(2).has_value());
}

TEST_F(LRUCacheTest, ThreadSafety) {
    LRUCache<int, int> shared_cache(1000);

    auto writer = [&shared_cache](int start) {
        for (int i = start; i < start + 100; ++i) {
            shared_cache.put(i, i * 10);
        }
    };

    auto reader = [&shared_cache](int start) {
        for (int i = start; i < start + 100; ++i) {
            shared_cache.get(i);
        }
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t) {
        threads.emplace_back(writer, t * 100);
        threads.emplace_back(reader, t * 100);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // No crash = thread safety works. Verify some data is present.
    EXPECT_GT(shared_cache.size(), 0u);
}

// ──────────────────────────────────────────────
// RouteCache tests
// ──────────────────────────────────────────────

class RouteCacheTest : public ::testing::Test {
protected:
    RouteCache cache{100};
};

TEST_F(RouteCacheTest, PutAndGet) {
    RouteResult result;
    result.found = true;
    result.total_duration_seconds = 500;
    result.algorithm = "Dijkstra";

    cache.put(1, 2, "Dijkstra", result);

    auto cached = cache.get(1, 2, "Dijkstra");
    ASSERT_TRUE(cached.has_value());
    EXPECT_EQ(cached->total_duration_seconds, 500u);
    EXPECT_TRUE(cached->found);
}

TEST_F(RouteCacheTest, DifferentAlgorithmsAreSeparate) {
    RouteResult r1;
    r1.found = true;
    r1.total_duration_seconds = 500;
    r1.algorithm = "Dijkstra";

    RouteResult r2;
    r2.found = true;
    r2.total_duration_seconds = 480;
    r2.algorithm = "A*";

    cache.put(1, 2, "Dijkstra", r1);
    cache.put(1, 2, "A*", r2);

    auto c1 = cache.get(1, 2, "Dijkstra");
    auto c2 = cache.get(1, 2, "A*");

    ASSERT_TRUE(c1.has_value());
    ASSERT_TRUE(c2.has_value());
    EXPECT_EQ(c1->total_duration_seconds, 500u);
    EXPECT_EQ(c2->total_duration_seconds, 480u);
}

TEST_F(RouteCacheTest, MissReturnsNullopt) {
    auto cached = cache.get(1, 2, "Dijkstra");
    EXPECT_FALSE(cached.has_value());
}

TEST_F(RouteCacheTest, DirectionMatters) {
    RouteResult r;
    r.found = true;
    r.total_duration_seconds = 500;

    cache.put(1, 2, "Dijkstra", r);

    // Reverse direction should miss
    EXPECT_FALSE(cache.get(2, 1, "Dijkstra").has_value());
}
