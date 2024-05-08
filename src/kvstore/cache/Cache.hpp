#ifndef E91CE472_FFC4_4CC0_B8C7_75DA44C82204
#define E91CE472_FFC4_4CC0_B8C7_75DA44C82204

#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "../DiskStorage.hpp"

namespace kvstore {

/**
 *  Abstract base class for an in-memory hash-map based (key, value) storage.
 *  The Cache supports get, put an remove operations.
 *  The size of the cache is bounded. Key-value pairs are evicted to disk when
 *  the size bound is exceeded. All logic relating to the eviction strategy of
 *  the cache is to be defined with the implementations of this class.
 */
class Cache {
   protected:
    /// Hash-map storing the key-value pairs stored in the cache
    std::unordered_map<std::string, std::string> kv_map;

    const uint64_t max_cache_size;

    /// The current size of the cache in bytes.
    /// Note: this only counts keys and values stored in the kv_map.
    /// Auxiliary storage needed for the eviction logic might consume
    /// additional space.
    uint64_t current_cache_size = 0;

    /// Mutex used for thread synchronization.
    mutable std::shared_mutex cache_mutex;
    /// Pointer to the disk-based storage to evict to on cache overflow.
    std::shared_ptr<DiskStorage> disk_storage;

    /* -- Strategy specific functions that outline the eviction logic -- */
    /// Function called when a key is accessed by the get operation.
    virtual void access_by_get(const std::string& key) = 0;
    /// Function called when a key is accessed by the put operation.
    virtual void access_by_put(const std::string& key) = 0;
    /**
     * Function is called when current_cache_size + cache_size_offset
     * is larger than max_cache_size.
     * The function evicts key-value pairs away from the cache to the
     * disk storage.
     * @param cache_size_offset size in bytes expected to be added
     * after the eviction.
     */
    virtual void perform_eviction(uint64_t cache_size_offset) = 0;
    /// Function is called with remove(key) is called. This ensures that
    /// the key is also removed from any auxiliary containers used for
    /// implementing the different cache strategies.
    virtual void strategy_specific_remove(const std::string& key) = 0;
    /* ----------------------------------------------------------------- */

   public:
    Cache(uint64_t max_cache_size, std::shared_ptr<DiskStorage> disk_storage);
    virtual ~Cache() = default;

    /// Fetches the value associated with the given key from the cache.
    /// Empty-string is returned if the key is not in the cache.
    std::string get(const std::string& key);

    /// Adds the given (key, value)-pair to the cache. An eviction might
    /// occur if the cache size would exceed the maximal allowed size.
    void put(const std::string& key, std::string&& value);

    /// Removes the key and its associated value from the cache.
    void remove(const std::string& key);
};
}  // namespace kvstore

#endif /* E91CE472_FFC4_4CC0_B8C7_75DA44C82204 */
