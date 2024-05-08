#ifndef C7D2BDE8_38E1_4C57_898D_9AD10E2A0735
#define C7D2BDE8_38E1_4C57_898D_9AD10E2A0735
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../kvstore/KVStore.hpp"

class ClientHandler {
   public:
    /// Run method for threads started to handle a client connection.
    static void run(int client_socket, kvstore::KVStore* kv_store,
                    std::size_t client_id) {
        char buffer[1024];

        // As long as the client connection is alive and requests are received.
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) {
                break;
            }

            std::string request(buffer);
            std::cout << "Received request from client " << client_id << " : "
                      << request << std::endl;
            std::istringstream request_ss(request);
            std::string command, key, value, response;

            // Here we assume that the user only sends valid requests.
            // TODO Handle invalid requests.
            request_ss >> command;
            if (command == "get") {  // get <key>
                request_ss >> key;
                auto res = kv_store->get(key);
                if (res.first == kvstore::RequestStatus::SUCCESS)
                    response = "Value for key '" + key + "': " + res.second;
                else
                    response = "Key not found";
            } else if (command == "put") {  // put <key> <value>
                request_ss >> key >> value;
                kv_store->put(key, std::string(value));
                response = "Key '" + key + "' set to value: " + value;
            } else if (command == "remove") {  // remove <key>
                request_ss >> key;
                kv_store->remove(key);
                response = "Key '" + key + "' removed.";
            } else {
                response = "Invalid command: " + request;
            }

            // Send the response back to the client
            send(client_socket, response.c_str(), response.size(), 0);
        }

        close(client_socket);
    }
};

#endif /* C7D2BDE8_38E1_4C57_898D_9AD10E2A0735 */
