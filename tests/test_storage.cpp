#include <catch2/catch_test_macros.hpp>
#include "redis/storage.hpp"
#include <string>

using namespace redis;

TEST_CASE("Storage: Basic SET and GET operations", "[storage]") {
    Storage storage;
    
    SECTION("Set multiple keys") {
        storage.set("key1", "value1");
        storage.set("key2", "value2");
        storage.set("key3", "value3");
        
        auto result1 = storage.get("key1");
        REQUIRE(result1.has_value());
        REQUIRE(result1.value().value() == "value1");
        
        auto result2 = storage.get("key2");
        REQUIRE(result2.has_value());
        REQUIRE(result2.value().value() == "value2");
        
        auto result3 = storage.get("key3");
        REQUIRE(result3.has_value());
        REQUIRE(result3.value().value() == "value3");
    }
    
    SECTION("Overwrite existing key") {
        storage.set("key1", "value1");

        REQUIRE(storage.exists("key1"));
        REQUIRE(storage.get("key1").has_value());
        REQUIRE(storage.get("key1").value().value() == "value1");

        storage.set("key1", "new_value");
        
        auto result = storage.get("key1");
        REQUIRE(result.has_value());
        REQUIRE(result.value().value() == "new_value");
    }
}

TEST_CASE("Storage: GET non-existent key", "[storage]") {
    Storage storage;
    
    auto result = storage.get("non_existent");
    
    REQUIRE(result.has_value());
    REQUIRE_FALSE(result.value().has_value()); // Should return nullopt
}

TEST_CASE("Storage: EXISTS operation", "[storage]") {
    Storage storage;
    
    SECTION("Key does not exist") {
        REQUIRE_FALSE(storage.exists("non_existent"));
    }
    
    SECTION("Key exists after SET") {
        storage.set("key1", "value1");
        REQUIRE(storage.exists("key1"));
    }
    
    SECTION("Key does not exist after DEL") {
        storage.set("key1", "value1");
        storage.del("key1");
        REQUIRE_FALSE(storage.exists("key1"));
    }
}

TEST_CASE("Storage: DEL operation", "[storage]") {
    Storage storage;
    
    SECTION("Delete existing key") {
        storage.set("key1", "value1");
        bool deleted = storage.del("key1");
        
        REQUIRE(deleted);
        REQUIRE_FALSE(storage.exists("key1"));
        
        auto result = storage.get("key1");
        REQUIRE(result.has_value());
        REQUIRE_FALSE(result.value().has_value());
    }
    
    SECTION("Delete non-existent key") {
        bool deleted = storage.del("non_existent");
        REQUIRE_FALSE(deleted);
    }
    
    SECTION("Delete multiple keys") {
        storage.set("key1", "value1");
        storage.set("key2", "value2");
        storage.set("key3", "value3");
        
        REQUIRE(storage.del("key1"));
        REQUIRE(storage.del("key2"));
        REQUIRE_FALSE(storage.exists("key1"));
        REQUIRE_FALSE(storage.exists("key2"));
        REQUIRE(storage.exists("key3"));
    }
    
    SECTION("Delete and recreate key") {
        storage.set("key1", "value1");
        storage.del("key1");
        storage.set("key1", "new_value");
        
        auto result = storage.get("key1");
        REQUIRE(result.has_value());
        REQUIRE(result.value().value() == "new_value");
    }
}
