# Test configuration with Catch2
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
)

FetchContent_MakeAvailable(Catch2)

# Protocol tests
add_executable(test_protocol test_protocol.cpp)
target_link_libraries(test_protocol PRIVATE Catch2::Catch2WithMain dumb_redis_cpp_lib)
target_include_directories(test_protocol PRIVATE ${CMAKE_SOURCE_DIR}/include)

include(CTest)
include(Catch)
Catch_discover_tests(test_protocol)

