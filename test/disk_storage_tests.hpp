#ifndef BF133F0D_2C9C_41FA_AD20_D6CA7B438E44
#define BF133F0D_2C9C_41FA_AD20_D6CA7B438E44

#include <cassert>
#include <filesystem>
#include <iostream>

#include "../src/kvstore/DiskStorage.hpp"

void test_disk_storage_simple() {
    auto disk_storage = kvstore::DiskStorage(0);

    assert(disk_storage.get("k1") == "");

    disk_storage.put_batch({{"k1", "v1"}, {"k2", "v2"}, {"k3", "v3"}});
    assert(disk_storage.get("k1") == "v1");
    assert(disk_storage.get("k2") == "v2");
    assert(disk_storage.get("k3") == "v3");

    disk_storage.put_batch({{"k4", "v4"}});
    assert(disk_storage.get("k1") == "v1");
    assert(disk_storage.get("k2") == "v2");
    assert(disk_storage.get("k3") == "v3");
    assert(disk_storage.get("k4") == "v4");

    disk_storage.put_batch({{"k1", "v1_new"}, {"k3", "v3_new"}});
    assert(disk_storage.get("k1") == "v1_new");  // overwritten
    assert(disk_storage.get("k2") == "v2");
    assert(disk_storage.get("k3") == "v3_new");  // overwritten
    assert(disk_storage.get("k4") == "v4");

    disk_storage.remove_batch({"k1", "k4"});
    assert(disk_storage.get("k1") == "");  // removed
    assert(disk_storage.get("k2") == "v2");
    assert(disk_storage.get("k3") == "v3_new");
    assert(disk_storage.get("k4") == "");  // removed
}

void run_disk_storage_tests() { test_disk_storage_simple(); }
#endif /* BF133F0D_2C9C_41FA_AD20_D6CA7B438E44 */
