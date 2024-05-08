#include "LRUCache.hpp"

#include <string>
#include <vector>

namespace kvstore {

void LRUCache::access_by_get(const std::string& key) {
    auto key_position_it = key_to_position.find(key);
    if (key_position_it == key_to_position.end()) {
        order.push_front(key);
        key_to_position[key] = order.begin();
    } else if (key_position_it->second != order.begin()) {
        assert(key_position_it->first == key);

        order.erase(key_position_it->second);
        order.push_front(key);
        key_to_position[key] = order.begin();
    }
}

void LRUCache::access_by_put(const std::string& key) { access_by_get(key); }

void LRUCache::perform_eviction(uint64_t cache_size_offset) {
    std::vector<std::pair<std::string, std::string>> entries_to_evict;

    uint64_t cache_size = this->current_cache_size;
    while (cache_size > 0 && cache_size + cache_size_offset > max_cache_size) {
        std::string key = std::move(order.back());
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

void LRUCache::strategy_specific_remove(const std::string& key) {
    auto key_position_entry = key_to_position.find(key);
    if (key_position_entry != key_to_position.end()) {
        order.erase(key_position_entry->second);
        key_to_position.erase(key_position_entry);
    }
}

}  // namespace kvstore
