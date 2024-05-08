#include "LFUCache.hpp"

#include <cassert>
#include <vector>

namespace kvstore {

void LFUCache::access_by_get(const std::string& key) {
    if (key_to_access_freq.find(key) == key_to_access_freq.end()) {
        // Although this is a Get call, we check if the key was
        // actually stored before since we want to re-use this
        // same implementation fo the Put call.

        key_to_access_freq[key] = 1;
        if (access_freq_to_keys.find(1) != access_freq_to_keys.end()) {
            assert(access_freq_to_keys[1].find(key) ==
                   access_freq_to_keys[1].end());
            access_freq_to_keys[1].insert(key);
        } else {
            access_freq_to_keys[1] = std::unordered_set<std::string>();
            access_freq_to_keys[1].insert(key);
        }
    } else {
        // The key is already in the cache. We have to update its
        // access frequency.

        // Update the key's access freq
        uint32_t old_access_freq = key_to_access_freq[key];
        uint32_t new_access_freq = old_access_freq + 1;
        key_to_access_freq[key] = new_access_freq;

        // Move the key to the hashset of the new access freq
        assert(access_freq_to_keys[new_access_freq].find(key) ==
               access_freq_to_keys[new_access_freq].end());
        access_freq_to_keys[new_access_freq].insert(key);
        access_freq_to_keys[old_access_freq].erase(key);

        // If the hashset corresponding to the old access freq is now
        // empty, we remove if from the map.
        if (access_freq_to_keys[old_access_freq].empty())
            access_freq_to_keys.erase(old_access_freq);
    }
}

void LFUCache::access_by_put(const std::string& key) { access_by_get(key); }

void LFUCache::perform_eviction(uint64_t cache_size_offset) {
    std::vector<std::pair<std::string, std::string>> entries_to_evict;

    uint64_t cache_size = this->current_cache_size;
    // Smallest frequency since we are using an ordered map.
    auto freq_it = access_freq_to_keys.begin();
    assert(freq_it != access_freq_to_keys.end());
    auto keys_it = freq_it->second.begin();
    assert(keys_it != freq_it->second.end());

    while (cache_size > 0 && cache_size + cache_size_offset > max_cache_size) {
        // We should reach cache_size == 0 before going through all freq lists
        assert(freq_it != access_freq_to_keys.end());

        std::string key = *keys_it;
        assert(kv_map.find(key) != kv_map.end());
        std::string value = std::move(kv_map[key]);
        kv_map.erase(key);

        // We can not do the strategy specific removals since the
        // iterators might get invalidated. Instead we perform the
        // removals of the evicted keys in the end.

        cache_size -= key.size() + value.size();

        entries_to_evict.push_back({std::move(key), std::move(value)});

        // Move to the next element in the same frequency list.
        freq_it++;

        // Move to the next frequency list if the current one is done
        if (keys_it == freq_it->second.end()) {
            freq_it++;
            keys_it = freq_it->second.begin();
        }
    }

    // Remove the keys to be evicted from the LFU auxiliary containers.
    for (const auto& entry : entries_to_evict)
        strategy_specific_remove(entry.first);

    this->current_cache_size = cache_size;
    disk_storage->put_batch(std::move(entries_to_evict));
}

void LFUCache::strategy_specific_remove(const std::string& key) {
    if (key_to_access_freq.find(key) != key_to_access_freq.end()) {
        uint32_t access_freq = key_to_access_freq[key];
        key_to_access_freq.erase(key);
        access_freq_to_keys[access_freq].erase(key);
        if (access_freq_to_keys[access_freq].empty())
            access_freq_to_keys.erase(access_freq);
    }
}

}  // namespace kvstore
