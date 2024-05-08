#ifndef D4EA05B3_1B88_4E46_9E2A_FF2288E9E25C
#define D4EA05B3_1B88_4E46_9E2A_FF2288E9E25C

#include <queue>

#include "Cache.hpp"

namespace kvstore {
/**
 * FIFO (First In First Out) implementation of the cache. When the cache is
 * full, keys are evicted in the order in which they were added to the cache.
 */
class FIFOCache : public Cache {
   private:
    /// FIFO Queue to keep track of the order in which the keys were
    /// added to the cache.
    /// Note: We use std::deque instead of std::queue for the erase method.
    std::deque<std::string> fifoQ;

    /// Does nothing.
    void access_by_get(const std::string& key) override;
    /// Adds the newly added key to the back of the fifo queue.
    void access_by_put(const std::string& key) override;
    /// Evict keys from the front of the queue.
    void perform_eviction(uint64_t cache_size_offset) override;
    /// Removes the key from the fifo queue.
    void strategy_specific_remove(const std::string& key) override;

   public:
    FIFOCache(uint64_t max_cache_size,
              std::shared_ptr<DiskStorage> disk_storage)
        : Cache(max_cache_size, disk_storage) {}
};
}  // namespace kvstore

#endif /* D4EA05B3_1B88_4E46_9E2A_FF2288E9E25C */
