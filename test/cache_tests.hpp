#ifndef CE0E2B02_BE10_40FF_839D_CFECED5DAD54
#define CE0E2B02_BE10_40FF_839D_CFECED5DAD54

#include <cassert>
#include <memory>

// TODO Fix cmake file such that no full paths are required
#include "../src/kvstore/DiskStorage.hpp"
#include "../src/kvstore/cache/Cache.hpp"
#include "../src/kvstore/cache/FIFOCache.hpp"
#include "../src/kvstore/cache/LFUCache.hpp"
#include "../src/kvstore/cache/LRUCache.hpp"

class DiskStorageMock : public kvstore::DiskStorage {
   public:
    DiskStorageMock(uint16_t id) : kvstore::DiskStorage(id) {}
    void put_batch(const std::vector<std::pair<std::string, std::string>>&&
                       kv_batch) override {}
};

void test_fifo_cache() {
    auto disk_storage_mock = std::make_shared<DiskStorageMock>(0);
    auto fifo_cache = kvstore::FIFOCache(/* Max size in bytes */ uint64_t{10},
                                         disk_storage_mock);

    fifo_cache.put("k1", "v1");
    fifo_cache.put("k2", "v2");

    assert(fifo_cache.get("k1") == "v1");
    assert(fifo_cache.get("k2") == "v2");
    assert(fifo_cache.get("k3") == "");

    fifo_cache.put("k1", "v1_");

    assert(fifo_cache.get("k1") == "v1_");
    assert(fifo_cache.get("k2") == "v2");

    fifo_cache.remove("k2");
    assert(fifo_cache.get("k1") == "v1_");
    assert(fifo_cache.get("k2") == "");

    fifo_cache.put("k2", "v2_");
    assert(fifo_cache.get("k1") == "v1_");
    assert(fifo_cache.get("k2") == "v2_");

    fifo_cache.put("k3", "v3");
    assert(fifo_cache.get("k1") == "");  // evicted
    assert(fifo_cache.get("k2") == "v2_");
    assert(fifo_cache.get("k3") == "v3");
}

void test_lru_cache() {
    auto disk_storage_mock = std::make_shared<DiskStorageMock>(0);
    auto lru_cache = kvstore::LRUCache(/* Max size in bytes */ uint64_t{10},
                                       disk_storage_mock);

    lru_cache.put("k1", "v1");
    lru_cache.put("k2", "v2");

    assert(lru_cache.get("k1") == "v1");
    assert(lru_cache.get("k2") == "v2");
    assert(lru_cache.get("k3") == "");

    lru_cache.put("k1", "v1_");

    assert(lru_cache.get("k1") == "v1_");
    assert(lru_cache.get("k2") == "v2");

    lru_cache.remove("k2");
    assert(lru_cache.get("k1") == "v1_");
    assert(lru_cache.get("k2") == "");

    lru_cache.put("k2", "v2_");
    assert(lru_cache.get("k1") == "v1_");
    assert(lru_cache.get("k2") == "v2_");

    lru_cache.put("k3", "v3");
    assert(lru_cache.get("k1") == "");  // evicted
    assert(lru_cache.get("k2") == "v2_");
    assert(lru_cache.get("k3") == "v3");

    lru_cache.get("k2");  // touch k2
    lru_cache.put("k4", "v4");
    assert(lru_cache.get("k1") == "");
    assert(lru_cache.get("k2") == "v2_");
    assert(lru_cache.get("k3") == "");  // evicted
    assert(lru_cache.get("k4") == "v4");
}

void test_lfu_cache() {
    auto disk_storage_mock = std::make_shared<DiskStorageMock>(0);
    auto lfu_cache = kvstore::LFUCache(/* Max size in bytes */ uint64_t{10},
                                       disk_storage_mock);

    lfu_cache.put("k1", "v1");
    lfu_cache.put("k2", "v2");

    assert(lfu_cache.get("k1") == "v1");
    assert(lfu_cache.get("k2") == "v2");
    assert(lfu_cache.get("k3") == "");  // Does not count as used

    lfu_cache.put("k1", "v1_");

    assert(lfu_cache.get("k1") == "v1_");
    assert(lfu_cache.get("k2") == "v2");

    lfu_cache.put("k2", "v2_");
    assert(lfu_cache.get("k1") == "v1_");
    assert(lfu_cache.get("k2") == "v2_");

    lfu_cache.get("k1");  // touch k1 -- now most frequently used
    lfu_cache.put("k3", "v3");
    assert(lfu_cache.get("k1") == "v1_");
    assert(lfu_cache.get("k2") == "");  // evicted
    assert(lfu_cache.get("k3") == "v3");

    lfu_cache.put("k4", "v4");
    assert(lfu_cache.get("k1") == "v1_");
    assert(lfu_cache.get("k2") == "");
    assert(lfu_cache.get("k3") == "");  // evicted
    assert(lfu_cache.get("k4") == "v4");
}

void run_cache_tests() {
    test_fifo_cache();
    test_lru_cache();
    test_lfu_cache();
}

#endif /* CE0E2B02_BE10_40FF_839D_CFECED5DAD54 */
