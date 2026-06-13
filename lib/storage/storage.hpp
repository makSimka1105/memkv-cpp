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
#include "util/hash.hpp"

namespace valkey::storage {

using TimePoint = std::chrono::steady_clock::time_point;

struct Entry {
    ValueVar value;
    std::optional<TimePoint> expires;
    size_t bytes = 0;
};

class Storage {
public:
    TimePoint Now() const { return std::chrono::steady_clock::now() + skew_; }

    void AdvanceClock(std::chrono::steady_clock::duration d) { skew_ += d; }

    void OnExpire(std::function<void(std::string_view, size_t)> fn) {
        on_expire_ = std::move(fn);
    }

    Entry& Set(std::string key, ValueVar value) {
        return map_.insert_or_assign(std::move(key), Entry{std::move(value), std::nullopt})
            .first->second;
    }

    Entry* GetMutable(std::string_view key) {
        auto it = FindLive(key);
        return it == map_.end() ? nullptr : &it->second;
    }

    const Entry* Get(std::string_view key) {
        auto it = FindLive(key);
        return it == map_.end() ? nullptr : &it->second;
    }

    bool Del(std::string_view key) {
        auto it = FindLive(key);
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

    bool Expire(std::string_view key, std::chrono::steady_clock::duration ttl) {
        auto it = FindLive(key);
        if (it == map_.end()) {
            return false;
        }
        it->second.expires = Now() + ttl;
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
                             *it->second.expires - Now())
                             .count();
        return remaining < 0 ? 0 : remaining;
    }

    size_t SweepExpired() {
        const auto now = Now();
        size_t removed = 0;
        for (auto it = map_.begin(); it != map_.end();) {
            if (it->second.expires && *it->second.expires <= now) {
                it = EvictExpired(it);
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
        auto it = map_.find(key);
        if (it == map_.end()) {
            return it;
        }
        if (it->second.expires && *it->second.expires <= Now()) {
            EvictExpired(it);
            return map_.end();
        }
        return it;
    }

    Map::iterator EvictExpired(Map::iterator it) {
        if (on_expire_) {
            on_expire_(it->first, it->second.bytes);
        }
        return map_.erase(it);
    }

    Map map_;
    std::chrono::steady_clock::duration skew_{};
    std::function<void(std::string_view, size_t)> on_expire_;
};

}  // namespace valkey::storage
