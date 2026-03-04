#pragma once

/// @file LRUCache.hpp
/// @brief Generic thread-safe LRU (Least Recently Used) cache.
/// @details O(1) get and put operations using a doubly-linked list + hash map.

#include <cstddef>
#include <list>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <unordered_map>
#include <utility>

namespace city_mapper {

/// @brief Generic LRU cache with configurable capacity.
/// @tparam Key   Cache key type (must be hashable).
/// @tparam Value Cache value type.
///
/// **Complexity:**
/// - `get()`:  O(1) amortized
/// - `put()`:  O(1) amortized
/// - `size()`: O(1)
///
/// **Thread safety:** Uses `std::shared_mutex` for concurrent reads,
/// exclusive writes. Safe for multi-threaded access.
template <typename Key, typename Value>
class LRUCache {
public:
    /// @brief Constructs an LRU cache with the specified capacity.
    /// @param capacity Maximum number of entries. Must be > 0.
    explicit LRUCache(size_t capacity) : capacity_(capacity > 0 ? capacity : 1) {}

    /// @brief Retrieves a value from the cache.
    /// @param key The key to look up.
    /// @return The cached value if present, std::nullopt otherwise.
    /// @details Moves the accessed entry to the front (most recently used).
    std::optional<Value> get(const Key& key) {
        std::unique_lock lock(mutex_);

        auto it = map_.find(key);
        if (it == map_.end()) {
            ++miss_count_;
            return std::nullopt;
        }

        ++hit_count_;

        // Move accessed entry to front of LRU list
        lru_list_.splice(lru_list_.begin(), lru_list_, it->second);
        return it->second->second;
    }

    /// @brief Inserts or updates a key-value pair in the cache.
    /// @param key   The key.
    /// @param value The value.
    /// @details If the cache is at capacity, evicts the least recently used entry.
    void put(const Key& key, Value value) {
        std::unique_lock lock(mutex_);

        auto it = map_.find(key);
        if (it != map_.end()) {
            // Update existing entry and move to front
            it->second->second = std::move(value);
            lru_list_.splice(lru_list_.begin(), lru_list_, it->second);
            return;
        }

        // Evict LRU entry if at capacity
        if (map_.size() >= capacity_) {
            auto& lru_entry = lru_list_.back();
            map_.erase(lru_entry.first);
            lru_list_.pop_back();
        }

        // Insert new entry at front
        lru_list_.emplace_front(key, std::move(value));
        map_[key] = lru_list_.begin();
    }

    /// @brief Checks if a key exists in the cache (without promoting it).
    [[nodiscard]] bool contains(const Key& key) const {
        std::shared_lock lock(mutex_);
        return map_.count(key) > 0;
    }

    /// @brief Returns the current number of cached entries.
    [[nodiscard]] size_t size() const {
        std::shared_lock lock(mutex_);
        return map_.size();
    }

    /// @brief Returns the maximum capacity.
    [[nodiscard]] size_t capacity() const noexcept { return capacity_; }

    /// @brief Clears all cached entries.
    void clear() {
        std::unique_lock lock(mutex_);
        map_.clear();
        lru_list_.clear();
        hit_count_ = 0;
        miss_count_ = 0;
    }

    /// @brief Returns the cache hit count (for monitoring).
    [[nodiscard]] uint64_t hit_count() const {
        std::shared_lock lock(mutex_);
        return hit_count_;
    }

    /// @brief Returns the cache miss count (for monitoring).
    [[nodiscard]] uint64_t miss_count() const {
        std::shared_lock lock(mutex_);
        return miss_count_;
    }

    /// @brief Returns the hit ratio (hits / total lookups).
    [[nodiscard]] double hit_ratio() const {
        std::shared_lock lock(mutex_);
        uint64_t total = hit_count_ + miss_count_;
        return total > 0 ? static_cast<double>(hit_count_) / static_cast<double>(total) : 0.0;
    }

private:
    using ListEntry = std::pair<Key, Value>;
    using ListIterator = typename std::list<ListEntry>::iterator;

    size_t capacity_;
    std::list<ListEntry> lru_list_;                  ///< Doubly-linked list for LRU ordering
    std::unordered_map<Key, ListIterator> map_;      ///< O(1) lookup into the list
    mutable std::shared_mutex mutex_;                ///< Read-write lock for thread safety
    uint64_t hit_count_{0};
    uint64_t miss_count_{0};
};

}  // namespace city_mapper
