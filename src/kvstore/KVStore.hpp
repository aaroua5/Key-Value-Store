#ifndef A8AE8624_BADD_4102_9352_AF316D439365
#define A8AE8624_BADD_4102_9352_AF316D439365

#include "DiskStorage.hpp"
#include "cache/Cache.hpp"

#define DEFAULT_CACHE_CAPACITY 10ull * 1024 * 1024  // 10 MB

namespace kvstore {

enum RequestStatus { SUCCESS, FAILURE };
enum CacheStrategy { FIFO, LFU, LRU };

/**
 * A Key-Value Store implementation consisting of an in-memory cache and a
 * disk-based persistent storage. The operations supported are:
 *   - get <key>: Fetches the value associated with the given key.
 *   - put <key> <value>: Adds the given key, value pair.
 *   - remove <key>: Removes the given key and its associated value.
 *
 * TODO Make the KVStore consist of several shards each with its own cache and
 * persistent store for a better parallelization.
 * Assign hashed key ranges to each shard.
 * In the current implementation, multiple clients can access the KVStore in
 * parallel but in practice the performance of processing requests is as good as
 * sequential in the worst case.
 */
class KVStore {
   private:
    /// Persistent KV Storage.
    std::shared_ptr<DiskStorage> kv_disk_storage;
    /// In-memory KV Storage
    std::unique_ptr<Cache> kv_cache;

   public:
    /**
     * Creates a KVStore with the given cache strategy and cache size limit
     * @param cache_strategy The eviction strategy of the cache (FIFO, LFU or
     * LRU).
     * @param cache_capacity The maximal size of the in-memory cache in bytes.
     */
    explicit KVStore(CacheStrategy cache_strategy,
                     uint64_t cache_capacity = DEFAULT_CACHE_CAPACITY);

    /// Adds the given key, value pair to the in-memory cache.
    /// @return SUCCESS if the pair was successfully inserted into the memory
    /// cache or FAILURE otherwise
    RequestStatus put(const std::string& key, std::string&& value);

    /// Fetches the value associated with the given key.
    /// First, the key is fetched from the cache. If it is not found, we fetch
    /// the key from the persistent store. If it is found, we insert it into the
    /// cache.
    /// @return {SUCCESS, <value>} if the key is found with its associated value
    /// or {FAILURE, -} if the key is not found.
    std::pair<RequestStatus, std::string> get(const std::string& key);

    /// Removes the entry with the given key from the cache and the persistent
    /// store.
    RequestStatus remove(const std::string& key);
};

}  // namespace kvstore

#endif /* A8AE8624_BADD_4102_9352_AF316D439365 */