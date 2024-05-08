#ifndef B6378735_EAED_4DC9_A2AB_1CD0CBE146A0
#define B6378735_EAED_4DC9_A2AB_1CD0CBE146A0

#include <map>
#include <unordered_map>
#include <unordered_set>

#include "Cache.hpp"

namespace kvstore {
/**
 * LFU (Least Frequently Used) implementation of the cache. When the cache is
 * full, keys that were the least accessed since their insertion into the cache
 * are evicted first.
 */
class LFUCache : public Cache {
   private:
    /// Ordered map mapping each access frequency f to a hash-set of keys that
    /// have been accessed exactly f times since their insertion to the cache.
    std::map<uint32_t, std::unordered_set<std::string>> access_freq_to_keys;
    /// Hash-map storing for each key in the cache how often it was accessed.
    std::unordered_map<std::string, uint32_t> key_to_access_freq;

    /// Increase the access frequency of the given key of set to 1 if the key
    /// has not yet been seen before.
    void access_by_get(const std::string& key) override;
    /// Increase the access frequency of the given key of set to 1 if the key
    /// has not yet been seen before.
    void access_by_put(const std::string& key) override;
    /// Evicts keys from the cache starting from those that have the lowest
    /// access frequency.
    void perform_eviction(uint64_t cacheSizeOffset) override;
    /// Removes the key from the LFU specific maps.
    void strategy_specific_remove(const std::string& key) override;

   public:
    LFUCache(uint64_t max_cache_size, std::shared_ptr<DiskStorage> disk_storage)
        : Cache(max_cache_size, disk_storage) {}
};

}  // namespace kvstore

#endif /* B6378735_EAED_4DC9_A2AB_1CD0CBE146A0 */
