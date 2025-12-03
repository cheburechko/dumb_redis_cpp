# Dumb Redis C++

A Redis clone implementation in C++ from scratch.

## Project Structure

```
dumb_redis_cpp/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── LICENSE                 # License file
├── include/                # Header files
│   └── redis/
│       ├── server.hpp      # Server and networking
│       ├── command.hpp     # Command parsing and execution
│       ├── storage.hpp     # Data storage engine
│       ├── protocol.hpp    # RESP protocol handling
│       ├── database.hpp    # Database operations
│       └── types.hpp       # Type definitions
├── src/                    # Source files
│   ├── main.cpp            # Entry point
│   ├── server.cpp          # Server implementation
│   ├── command.cpp         # Command implementation
│   ├── storage.cpp         # Storage implementation
│   ├── protocol.cpp        # Protocol implementation
│   └── database.cpp        # Database implementation
└── tests/                  # Test files
    └── CMakeLists.txt      # Test build configuration
```

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./dumb_redis_cpp
```

## Testing

```bash
cd build
ctest
```

## Features (Planned)

- [ ] RESP protocol support
- [ ] Basic data structures (strings, lists, sets, hashes)
- [ ] Command parsing and execution
- [ ] Network server with TCP support
- [ ] Client connections handling
- [ ] Persistence (optional)

