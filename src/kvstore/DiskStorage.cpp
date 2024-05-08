#include "DiskStorage.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace kvstore {

DiskStorage::DiskStorage(uint16_t id, std::string directory) {
    std::filesystem::path directory_path(directory);
    assert(directory_path.is_absolute() &&
           std::filesystem::is_directory(directory_path) &&
           std::filesystem::exists(directory_path));
    this->file_path = directory_path / (DEFAULT_DISK_STORAGE_FILENAME_PREFIX +
                                        std::to_string(id));
    this->aux_file_path =
        directory_path /
        (DEFAULT_DISK_STORAGE_FILENAME_PREFIX + std::to_string(id) + "_aux");

    // Clear out the content of the files.
    std::ofstream file(file_path, std::ios::trunc);
    file.close();
    std::ofstream aux_file(aux_file_path, std::ios::trunc);
    aux_file.close();
}

void DiskStorage::put_batch(
    const std::vector<std::pair<std::string, std::string>>&& kv_batch) {
    // First remove the keys from the file so that they are later overwritten.
    std::unordered_set<std::string> keys;
    for (auto entry : kv_batch) {
        keys.insert(entry.first);
    }
    remove_batch(keys);

    // Exclusively lock the disk based storage.
    std::unique_lock<std::shared_mutex> lock(disk_mutex);

    // Open the storage file in append mode.
    std::ofstream fout(file_path, std::ios::app);
    assert(fout.is_open());

    // Add the (key, value) pairs to the end of the file.
    // One pair per line.
    for (const auto& kv_entry : kv_batch)
        fout << kv_entry.first + delimiter + kv_entry.second << '\n';

    fout.close();
}

std::string DiskStorage::get(const std::string& key) {
    // Non-exclusive lock for readers.
    std::shared_lock<std::shared_mutex> lock(disk_mutex);

    std::ifstream fin(this->file_path);
    assert(fin.is_open());

    // Here we read the content of the file line by line
    // until EOF is reached or we find the given key.
    std::string line;
    while (!fin.eof() && std::getline(fin, line)) {
        auto delimiter_pos = line.find(delimiter);
        assert(delimiter_pos != std::string::npos);

        std::string line_key = line.substr(0, delimiter_pos);
        if (line_key == key) {
            // Key is found, we close the file and return the value from the
            // file.
            fin.close();
            return line.substr(delimiter_pos + 1);
        }
    }

    // Key was not found in the file. Close the file and return the empty
    // string.
    fin.close();
    return "";
}

void DiskStorage::remove(const std::string& key) {
    std::unordered_set<std::string> keys{key};
    remove_batch(keys);
}

void DiskStorage::remove_batch(const std::unordered_set<std::string>& keys) {
    // Exclusive lock
    std::unique_lock<std::shared_mutex> lock(disk_mutex);

    std::ifstream fin(this->file_path);
    std::ofstream fout(this->aux_file_path);

    assert(fin.is_open() && fout.is_open());

    std::string line;

    // Read one line (i.e. key,value pair at a time).
    while (!fin.eof() && std::getline(fin, line)) {
        auto delimiter_pos = line.find(delimiter);
        assert(delimiter_pos != std::string::npos);

        std::string key = line.substr(0, delimiter_pos);
        // if the key is not to be removed we write the line to the aux file.
        if (keys.find(key) == keys.end()) fout << line << '\n';
    }
    fin.close();
    fout.close();

    // Replace the storage file with the aux file
    std::rename(aux_file_path.c_str(), file_path.c_str());

    // Re-create the aux file
    std::ofstream aux_file(aux_file_path, std::ios::trunc);
    aux_file.close();
}

}  // namespace kvstore
