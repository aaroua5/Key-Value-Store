#ifndef BBBB0A07_E317_4838_8CB8_5744D1DEB8DB
#define BBBB0A07_E317_4838_8CB8_5744D1DEB8DB

#include "../src/kvstore/KVStore.hpp"

void _test_kv_store(kvstore::KVStore& kv_store) {
    for (int i = 0; i < 16; i++)
        kv_store.put("k" + std::to_string(i), "v" + std::to_string(i));

    for (int i = 0; i < 16; i++) {
        auto resp = kv_store.get("k" + std::to_string(i));
        assert(resp.first == kvstore::RequestStatus::SUCCESS);
        assert(resp.second == "v" + std::to_string(i));
    }

    assert(kv_store.get("whatever").first == kvstore::RequestStatus::FAILURE);

    for (int i = 0; i < 16; i++)
        if (i & 1) kv_store.remove("k" + std::to_string(i));

    for (int i = 0; i < 16; i++) {
        auto resp = kv_store.get("k" + std::to_string(i));
        if (i & 1)
            assert(resp.first == kvstore::RequestStatus::FAILURE);
        else
            assert(resp.first == kvstore::RequestStatus::SUCCESS &&
                   resp.second == "v" + std::to_string(i));
    }
}

void test_kv_store_with_fifo_cache() {
    auto kv_store = kvstore::KVStore(kvstore::CacheStrategy::FIFO, 10ull);
    _test_kv_store(kv_store);
}

void test_kv_store_with_lru_cache() {
    auto kv_store = kvstore::KVStore(kvstore::CacheStrategy::LRU, 10ull);
    _test_kv_store(kv_store);
}

void test_kv_store_with_lfu_cache() {
    auto kv_store = kvstore::KVStore(kvstore::CacheStrategy::LFU, 10ull);
    _test_kv_store(kv_store);
}

void run_kvstore_tests() {
    test_kv_store_with_fifo_cache();
    test_kv_store_with_lru_cache();
    test_kv_store_with_lfu_cache();
}
#endif /* BBBB0A07_E317_4838_8CB8_5744D1DEB8DB */
