#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

#include "../kvstore/KVStore.hpp"
#include "ClientHandler.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./KVStore_Server <server-port>" << std::endl;
        return 1;
    }

    // Create KV-Store for the clients to access and modify.
    kvstore::KVStore kv_store = kvstore::KVStore(kvstore::CacheStrategy::LRU);

    int server_port = atoi(argv[1]);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Failed to create a socket." << std::endl;
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr*)&server_addr,
             sizeof(server_addr)) == -1) {
        std::cerr << "Failed to bind the server socket. " << strerror(errno)
                  << std::endl;
        close(server_socket);
        return 1;
    }

    listen(server_socket, 5);

    std::vector<std::thread> client_threads;
    std::size_t client_cnt = 0;

    // In a loop, listen for new client connections and create a handler thread
    // for each.
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int client_socket = accept(
            server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            std::cerr << "Failed to accept a client connection." << std::endl;
            continue;
        }
        std::cout << "Accepted connection to client " << client_cnt
                  << std::endl;

        // Create a new thread to handle the client connection
        client_threads.emplace_back(&ClientHandler::run, client_socket,
                                    &kv_store, client_cnt++);
    }

    for (auto& thread : client_threads) {
        thread.join();
    }

    close(server_socket);

    return 0;
}