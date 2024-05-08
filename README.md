We provide a CMake project for a KVStore library and a basic client and server implementation.
The KVStore consists of:
- In-memory Cache (eviction strategies currently supported are FIFO, LFU and LRU).
- Persistent disk storage: File based store (by default under /tmp/ directory). 
The KVStore is thread-safe.
We provide a network interface for a server responsible for a KVStore and a client able to connect to a server and access and modify its KVStore.


# Building
mkdir build
cd build
cmake ..
make

# Usage
KVStore_Tests: Runs all KVStore-related tests.
KVStore_Server <server-port>: Starts a server at the given port. The server is responsible for a KVStore instance and accepts multiple client connections concurrently that can access and modify the KVStore.
KVStore_Client <server-ip> <server-port>: Starts a client and establishes connection to a KVStore server running at the given ip and port.

# Example
- Server:
```
└─$ ./KVStore_Server 8990
Accepted connection to client 0
Accepted connection to client 1
Received request from client 0 : get k1
Received request from client 0 : put k1 v1
Received request from client 1 : put k2 v2
Received request from client 1 : get k1
Received request from client 1 : get k2
Received request from client 0 : get k1
Received request from client 0 : get k2
Received request from client 1 : remove k1
Received request from client 0 : get k1
Received request from client 1 : get k1
Received request from client 1 : get k2
Received request from client 0 : get k2
```

- Client 0:
```
└─$ ./KVStore_Client 127.0.0.1 8990                                                  
Connection to server successfully established.
> get k1
Server response: Key not found
> put k1 v1
Server response: Key 'k1' set to value: v1
> get k1
Server response: Value for key 'k1': v1
> get k2
Server response: Value for key 'k2': v2
> get k1
Server response: Key not found
> get k2
Server response: Value for key 'k2': v2
> exit
```

- Client 1:
```
└─$ ./KVStore_Client 127.0.0.1 8990                                                  
Connection to server successfully established.
> put k2 v2
Server response: Key 'k2' set to value: v2
> get k1
Server response: Value for key 'k1': v1
> get k2
Server response: Value for key 'k2': v2
> remove k1
Server response: Key 'k1' removed.
> get k1
Server response: Key not found
> get k2
Server response: Value for key 'k2': v2
> exit
```
# Key-Value-Store
