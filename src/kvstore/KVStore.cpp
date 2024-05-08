#include "KVStore.hpp"

#include <iostream>

#include "cache/FIFOCache.hpp"
#include "cache/LFUCache.hpp"
#include "cache/LRUCache.hpp"

namespace kvstore {

KVStore::KVStore(CacheStrategy cache_strategy, uint64_t cache_capacity) {
    kv_disk_storage = std::make_shared<DiskStorage>(0);
    switch (cache_strategy) {
        case FIFO:
            kv_cache = std::make_unique<FIFOCache>(DEFAULT_CACHE_CAPACITY,
                                                   kv_disk_storage);
            break;
        case LRU:
            kv_cache = std::make_unique<LRUCache>(DEFAULT_CACHE_CAPACITY,
                                                  kv_disk_storage);
            break;
        case LFU:
            kv_cache = std::make_unique<LFUCache>(DEFAULT_CACHE_CAPACITY,
                                                  kv_disk_storage);
            break;
        default:
            assert(false);
            break;
    }
}

RequestStatus KVStore::put(const std::string& key, std::string&& value) {
    kv_cache->put(key, std::move(value));
    return SUCCESS;
}

std::pair<RequestStatus, std::string> KVStore::get(const std::string& key) {
    auto result = kv_cache->get(key);
    if (result == "") {
        result = kv_disk_storage->get(key);

        if (result == "") return {FAILURE, ""};

        kv_cache->put(key, std::string(result));
    }
    return {SUCCESS, result};
}

RequestStatus KVStore::remove(const std::string& key) {
    kv_cache->remove(key);
    kv_disk_storage->remove(key);

    return SUCCESS;
}

}  // namespace kvstore
