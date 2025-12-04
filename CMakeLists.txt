cmake_minimum_required(VERSION 3.20)
project(dumb_redis_cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic --stdlib=libc++")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files (excluding main.cpp for library)
set(LIB_SOURCES
    src/server.cpp
    src/client_connection.cpp
    src/command.cpp
    src/storage.cpp
    src/protocol.cpp
    src/database.cpp
)

set(EXEC_SOURCES
    src/main.cpp
    ${LIB_SOURCES}
)

# Header files
set(HEADERS
    include/redis/server.hpp
    include/redis/client_connection.hpp
    include/redis/command.hpp
    include/redis/storage.hpp
    include/redis/protocol.hpp
    include/redis/database.hpp
    include/redis/types.hpp
)

# Create library for linking with tests
add_library(${PROJECT_NAME}_lib ${LIB_SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME}_lib PUBLIC pthread)

# Create executable
add_executable(${PROJECT_NAME} ${EXEC_SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} pthread)

# Testing
enable_testing()
add_subdirectory(tests)

