#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

/// Function used to establish a connection to a server given its ip and port.
/// SocketId is returned.
int create_connection_to_server(hostent* server_ip, int server_port) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        std::cerr << "Failed to create a socket." << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    bcopy((char*)server_ip->h_addr, (char*)&server_addr.sin_addr,
          server_ip->h_length);

    if (connect(client_socket, (struct sockaddr*)&server_addr,
                sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect to the server." << std::endl;
        close(client_socket);
        return -1;
    }

    return client_socket;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./KVStore_Client <server-ip> <server-port>"
                  << std::endl;
        return -1;
    }

    auto server_ip = gethostbyname(argv[1]);
    if (server_ip == NULL) {
        std::cerr << "Invalid server IP address." << std::endl;
        return -1;
    }
    auto server_port = atoi(argv[2]);

    auto client_socket = create_connection_to_server(server_ip, server_port);
    if (client_socket < 0) {
        return -1;
    }

    std::cout << "Connection to server successfully established." << std::endl;

    std::string command;
    char response[1024];
    memset(response, 0, sizeof(response));

    // Forward user input from stdin to the server and print out the server's
    // response until the user enters 'exit'
    while (true) {
        memset(response, 0, sizeof(response));

        std::cout << "> ";
        // Read the next request from stdin
        std::getline(std::cin, command);
        // Break out of the loop on 'exit'
        if (command == "exit") break;

        // Send request to server
        send(client_socket, command.c_str(), command.size(), 0);

        // Read server's response
        int bytes_received = recv(client_socket, response, sizeof(response), 0);
        if (bytes_received < 0) {
            std::cerr << "Failed to receive a response from the server."
                      << std::endl;
            break;
        }

        // Print server's response
        std::cout << "Server response: " << response << std::endl;
    }

    close(client_socket);

    return 0;
}