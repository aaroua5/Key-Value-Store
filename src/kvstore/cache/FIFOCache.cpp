#include "FIFOCache.hpp"

namespace kvstore {

void FIFOCache::access_by_get(const std::string& key) {}

void FIFOCache::access_by_put(const std::string& key) { fifoQ.push_back(key); }

void FIFOCache::perform_eviction(uint64_t cache_size_offset) {
    std::vector<std::pair<std::string, std::string>> entries_to_evict;

    uint64_t cache_size = this->current_cache_size;
    while (cache_size > 0 && cache_size + cache_size_offset > max_cache_size) {
        std::string key = std::move(fifoQ.front());
        assert(kv_map.find(key) != kv_map.end());
        std::string value = std::move(kv_map[key]);
        kv_map.erase(key);
        strategy_specific_remove(key);

        cache_size -= key.size() + value.size();

        entries_to_evict.push_back({std::move(key), std::move(value)});
    }

    this->current_cache_size = cache_size;
    disk_storage->put_batch(std::move(entries_to_evict));
}

void FIFOCache::strategy_specific_remove(const std::string& key) {
    for (auto it = fifoQ.begin(); it != fifoQ.end(); it++) {
        if (*it == key) fifoQ.erase(it);
        return;
    }
}

}  // namespace kvstore
