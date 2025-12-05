#include "redis/storage.hpp"
#include <algorithm>

namespace redis {

Storage::Storage() {
}

Storage::~Storage() {
}

void Storage::set(const std::string& key, const std::string& value) {
    // TODO: Implement SET operation
    data_[key] = value;
}

std::expected<std::optional<std::string>, std::string> Storage::get(const std::string& key) {
    // TODO: Implement GET operation
    auto it = data_.find(key);
    if (it == data_.end()) {
        return std::nullopt;
    }
    if (std::holds_alternative<std::string>(it->second)) {
        return std::get<std::string>(it->second);
    }
    return std::unexpected("Value is not a string");
}

bool Storage::del(const std::string& key) {
    // TODO: Implement DEL operation
    return data_.erase(key) > 0;
}

bool Storage::exists(const std::string& key) {
    // TODO: Implement EXISTS operation
    return data_.contains(key);
}

size_t Storage::size() const {
    return data_.size();
}

void Storage::clear() {
    data_.clear();
}

ValueType Storage::getValueType(const std::string& key) const {
    // TODO: Implement value type checking
    return ValueType::NONE;
}

} // namespace redis

