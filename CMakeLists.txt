cmake_minimum_required(VERSION 3.25)
project(KVStore)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE KVSTORE_SRC
    "src/kvstore/*.hpp"
    "src/kvstore/*.cpp"
    "src/kvstore/cache/*.hpp"
    "src/kvstore/cache/*.cpp"
)

add_executable(KVStore_Tests 
    ${KVSTORE_SRC}
    test/main.cpp 
)

add_executable(KVStore_Client
    src/client/main.cpp
)

add_executable(KVStore_Server
    ${KVSTORE_SRC}
    src/server/main.cpp
)
