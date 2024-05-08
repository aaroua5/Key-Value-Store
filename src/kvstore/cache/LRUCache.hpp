#ifndef C2358896_D01D_42D5_B55C_BAF5BC0D491E
#define C2358896_D01D_42D5_B55C_BAF5BC0D491E

#include <list>
#include <unordered_map>

#include "Cache.hpp"

namespace kvstore {
/**
 * LRU (Least Recently Used) implementation of the cache. When the cache is
 * full, keys that were not accessed the longest are evicted from the cache.
 */
class LRUCache : public Cache {
   private:
    /// Maps each key to the list element in the access order list.
    std::unordered_map<std::string, std::list<std::string>::iterator>
        key_to_position;
    /// Linked-list storing the order in which the keys have been most recently
    /// accessed. Front: most-recently accessed; Back: least-recently accessed.
    std::list<std::string> order;

    /// Brings the given key to the front of the access order list and update
    /// the key to list element mapping accordingly.
    void access_by_get(const std::string& key) override;
    /// Brings the given key to the front of the access order list and update
    /// the key to list element mapping accordingly.
    void access_by_put(const std::string& key) override;
    /// Evicts keys from the cache starting from the back of the access order
    /// list.
    void perform_eviction(uint64_t cache_size_offset) override;
    /// Removes the key from the access order list and the key to list element
    /// mapping.
    void strategy_specific_remove(const std::string& key) override;

   public:
    LRUCache(uint64_t max_cache_size, std::shared_ptr<DiskStorage> disk_storage)
        : Cache(max_cache_size, disk_storage) {}
};

}  // namespace kvstore

#endif /* C2358896_D01D_42D5_B55C_BAF5BC0D491E */
