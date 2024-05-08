#include "cache_tests.hpp"
#include "disk_storage_tests.hpp"
#include "kvstore_tests.hpp"

int main() {
    run_cache_tests();
    run_disk_storage_tests();
    run_kvstore_tests();
}