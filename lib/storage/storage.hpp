#pragma once

#include <chrono>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "storage/value.hpp"

namespace valkey::storage {

using ChronoTimePoint = std::chrono::steady_clock::time_point;

struct ValueWithMeta {
    ValueVar value;
    std::optional<ChronoTimePoint> expires;
};

struct StringHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }
    size_t operator()(const std::string& s) const noexcept {
        return std::hash<std::string_view>{}(s);
    }
    size_t operator()(const char* s) const noexcept { return std::hash<std::string_view>{}(s); }
};

namespace detail {

bool GlobMatch(std::string_view pattern, std::string_view text) {
    if (pattern.empty()) {
        return text.empty();
    }
    if (pattern[0] == '*') {
        for (size_t i = 0; i <= text.size(); ++i) {
            if (GlobMatch(pattern.substr(1), text.substr(i))) {
                return true;
            }
        }
        return false;
    }
    if (text.empty()) {
        return false;
    }
    if (pattern[0] == '?' || pattern[0] == text[0]) {
        return GlobMatch(pattern.substr(1), text.substr(1));
    }
    return false;
}

}  // namespace detail

class Storage {
public:
    bool Set(std::string key, ValueVar value) {
        auto [it, inserted] =
            map_.insert_or_assign(std::move(key), ValueWithMeta{std::move(value), std::nullopt});
        return inserted;
    }

    ValueVar* GetMutable(std::string_view key,
                         ChronoTimePoint now = std::chrono::steady_clock::now()) {
        auto it = FindLive(key, now);
        return it == map_.end() ? nullptr : &it->second.value;
    }

    const ValueVar* Get(std::string_view key,
                        ChronoTimePoint now = std::chrono::steady_clock::now()) {
        auto it = FindLive(key, now);
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

    bool Exists(std::string_view key) const { return map_.contains(key); }
    size_t DbSize() const { return map_.size(); }
    void FlushDb() { map_.clear(); }

    bool Expire(std::string_view key, ChronoTimePoint when) {
        auto it = map_.find(key);
        if (it == map_.end()) {
            return false;
        }
        it->second.expires = when;
        return true;
    }

    size_t TtlSeconds(std::string_view key,
                      ChronoTimePoint now = std::chrono::steady_clock::now()) const {
        auto it = map_.find(key);
        if (it == map_.end()) {
            return static_cast<size_t>(-2);
        }
        if (!it->second.expires) {
            return static_cast<size_t>(-1);
        }
        auto remaining =
            std::chrono::duration_cast<std::chrono::seconds>(*it->second.expires - now).count();
        return remaining < 0 ? 0 : static_cast<size_t>(remaining);
    }

    size_t SweepExpired(ChronoTimePoint now = std::chrono::steady_clock::now()) {
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

    std::vector<std::string> Keys(std::string_view pattern,
                                  ChronoTimePoint now = std::chrono::steady_clock::now()) {
        std::vector<std::string> result;
        for (auto it = map_.begin(); it != map_.end();) {
            if (it->second.expires && *it->second.expires <= now) {
                it = map_.erase(it);
                continue;
            }
            if (detail::GlobMatch(pattern, it->first)) {
                result.push_back(it->first);
            }
            ++it;
        }
        return result;
    }

private:
    using Map = std::unordered_map<std::string, ValueWithMeta, StringHash, std::equal_to<>>;

    Map::iterator FindLive(std::string_view key, ChronoTimePoint now) {
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
