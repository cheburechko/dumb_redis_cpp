# Test configuration with Catch2
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
)

FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
  hiredis
  GIT_REPOSITORY https://github.com/redis/hiredis
  GIT_TAG v1.3.0
  GIT_SHALLOW TRUE
  SOURCE_DIR _deps/hiredis
  # Make sure redis++ finds fetched content with its find_package()
  OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
  redis-plus-plus
  GIT_REPOSITORY https://github.com/sewenew/redis-plus-plus.git
  GIT_TAG 1.3.15
  GIT_SHALLOW TRUE
)

# Make redis-plus-plus available (it will find hiredis)
FetchContent_MakeAvailable(redis-plus-plus)

# Protocol tests
add_executable(test_protocol test_protocol.cpp)
target_link_libraries(test_protocol PRIVATE Catch2::Catch2WithMain dumb_redis_cpp_lib)
target_include_directories(test_protocol PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Server integration tests
add_executable(test_server_integration test_server_integration.cpp)
target_link_libraries(
    test_server_integration 
    PRIVATE 
    Catch2::Catch2WithMain 
    dumb_redis_cpp_lib
    redis++
    pthread
)
target_include_directories(test_server_integration PRIVATE ${CMAKE_SOURCE_DIR}/include)

include(CTest)
include(Catch)
Catch_discover_tests(test_protocol)
Catch_discover_tests(test_server_integration)

