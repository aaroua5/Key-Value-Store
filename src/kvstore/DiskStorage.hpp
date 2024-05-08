#ifndef A083461B_DCB4_44C5_B2AA_11D74ABA5F90
#define A083461B_DCB4_44C5_B2AA_11D74ABA5F90

#define DEFAULT_DISK_STORAGE_DIRECTORY "/tmp/"
#define DEFAULT_DISK_STORAGE_FILENAME_PREFIX "kvstore_disk_file_"

#include <filesystem>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_set>
#include <vector>

namespace kvstore {

/**
 * A basic disk-based (key,value)-store. The key-value pairs are stored in a
 * file. One pair per line. Key and value are separated by a comma.
 */
class DiskStorage {
   private:
    /// Main file used to store the key-value pairs
    std::filesystem::path file_path;
    /// An auxiliary file used as an intermediary storage when overwriting the
    /// content of the main file.
    std::filesystem::path aux_file_path;

    /// Mutex used for thread synchronization.
    mutable std::shared_mutex disk_mutex;

    /// Character used to separate the key and value in each line in the file.
    const char delimiter = ',';

   public:
    DiskStorage(uint16_t id,
                std::string directory = DEFAULT_DISK_STORAGE_DIRECTORY);
    /// Adds the given array of key, value pairs to the file.
    virtual void put_batch(
        const std::vector<std::pair<std::string, std::string>>&& kv_batch);
    /// Searches for the given key in the file and returns its associated value
    /// if it is found. Otherwise, the empty string is returned.
    std::string get(const std::string& key);
    /// Remove the entry associated with the given key from the file.
    void remove(const std::string& key);
    /// Remove all entries associated with any of the given keys from the file.
    void remove_batch(const std::unordered_set<std::string>& keys);
};

}  // namespace kvstore

#endif /* A083461B_DCB4_44C5_B2AA_11D74ABA5F90 */
