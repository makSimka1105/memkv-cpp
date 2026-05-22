#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "storage/glob.hpp"
#include "storage/value.hpp"

namespace valkey::storage {

using TimePoint = std::chrono::steady_clock::time_point;

struct Entry {
    ValueVar value;
    std::optional<TimePoint> expires;
};

struct StringHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }
};

class Storage {
public:
    void Set(std::string key, ValueVar value) {
        map_.insert_or_assign(std::move(key), Entry{std::move(value), std::nullopt});
    }

    ValueVar* GetMutable(std::string_view key) {
        auto it = FindLive(key);
        return it == map_.end() ? nullptr : &it->second.value;
    }

    const ValueVar* Get(std::string_view key) {
        auto it = FindLive(key);
        return it == map_.end() ? nullptr : &it->second.value;
    }

    bool Del(std::string_view key) {
        auto it = map_.find(key);
        if (it == map_.end()) {
            return false;
        }
        map_.erase(it);
        return true;
    }

    bool Exists(std::string_view key) { return FindLive(key) != map_.end(); }

    size_t DbSize() {
        SweepExpired();
        return map_.size();
    }

    void FlushDb() { map_.clear(); }

    bool Expire(std::string_view key, TimePoint when) {
        auto it = map_.find(key);
        if (it == map_.end()) {
            return false;
        }
        it->second.expires = when;
        return true;
    }

    int64_t TtlSeconds(std::string_view key) {
        auto it = FindLive(key);
        if (it == map_.end()) {
            return -2;
        }
        if (!it->second.expires) {
            return -1;
        }
        auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
                             *it->second.expires - std::chrono::steady_clock::now())
                             .count();
        return remaining < 0 ? 0 : remaining;
    }

    size_t SweepExpired() {
        const auto now = std::chrono::steady_clock::now();
        size_t removed = 0;
        for (auto it = map_.begin(); it != map_.end();) {
            if (it->second.expires && *it->second.expires <= now) {
                it = map_.erase(it);
                ++removed;
            } else {
                ++it;
            }
        }
        return removed;
    }

    std::vector<std::string> Keys(std::string_view pattern) {
        SweepExpired();
        std::vector<std::string> result;
        for (const auto& [key, _] : map_) {
            if (GlobMatch(pattern, key)) {
                result.push_back(key);
            }
        }
        return result;
    }

private:
    using Map = std::unordered_map<std::string, Entry, StringHash, std::equal_to<>>;

    Map::iterator FindLive(std::string_view key) {
        const auto now = std::chrono::steady_clock::now();
        auto it = map_.find(key);
        if (it == map_.end()) {
            return it;
        }
        if (it->second.expires && *it->second.expires <= now) {
            map_.erase(it);
            return map_.end();
        }
        return it;
    }

    Map map_;
};

}  // namespace valkey::storage
