#include "Cache.hpp"

#include <cassert>
#include <shared_mutex>

namespace kvstore {

Cache::Cache(uint64_t max_cache_size, std::shared_ptr<DiskStorage> disk_storage)
    : max_cache_size(max_cache_size), disk_storage(disk_storage) {}

std::string Cache::get(const std::string& key) {
    /// We use a shared lock for get to allow concurrent reads.
    std::shared_lock<std::shared_mutex> lock(cache_mutex);

    if (kv_map.find(key) != kv_map.end()) {
        // Eviction-strategy-specific logic when a key is accessed by get.
        access_by_get(key);
        return kv_map[key];
    }

    return "";  // TODO change function return type
}

void Cache::put(const std::string& key, std::string&& value) {
    // Exclusive lock. No concurrent reads/writes.
    std::unique_lock<std::shared_mutex> lock(cache_mutex);

    // Whether the key already exists in the cache and its value is
    // being updated
    bool is_update = kv_map.find(key) != kv_map.end();

    // We might evict if the key is not in the cache yet or this is an
    // update to a larger (in terms of size) value.
    if (!is_update || (is_update && kv_map[key].size() < value.size())) {
        uint64_t cache_size_offset = key.size() + value.size();
        if (is_update) cache_size_offset -= kv_map[key].size() + key.size();
        uint64_t new_cache_size = current_cache_size + cache_size_offset;
        if (new_cache_size > max_cache_size) {
            perform_eviction(cache_size_offset);
        }
    }

    // Update the cache size.
    current_cache_size += key.size() + value.size() -
                          (is_update ? kv_map[key].size() + key.size() : 0);
    assert(current_cache_size <= max_cache_size);

    // Eviction-strategy-specific logic when a key is accessed by put.
    access_by_put(key);
    kv_map[key] = std::move(value);
}

void Cache::remove(const std::string& key) {
    // Exclusive lock. No concurrent reads/writes.
    std::unique_lock<std::shared_mutex> lock(cache_mutex);

    if (kv_map.find(key) != kv_map.end()) {
        // Update the cache size.
        current_cache_size -= key.size() + kv_map[key].size();
        kv_map.erase(key);
        // We also remove the key from strategy-specific auxiliary storage.
        strategy_specific_remove(key);
    }
}

}  // namespace kvstore