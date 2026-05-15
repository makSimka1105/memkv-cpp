#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <variant>

#include "storage/value.hpp"

namespace valkey::engine {

constexpr size_t SizeOf(const valkey::storage::Stringy auto& s) {
    return sizeof(s) + s.size();
}

size_t SizeOf(const valkey::storage::List& lst) {
    size_t total = sizeof(valkey::storage::List);
    for (const auto& s : lst) {
        total += sizeof(std::string) + s.size();
    }
    return total;
}

size_t SizeOf(const valkey::storage::Set& s) {
    size_t total = sizeof(valkey::storage::Set);
    for (const auto& e : s) {
        total += sizeof(std::string) + e.size();
    }
    return total;
}

size_t SizeOf(const valkey::storage::ValueVar& v) {
    if (const auto* s = std::get_if<valkey::storage::String>(&v)) {
        return sizeof(valkey::storage::String) + s->size();
    }
    if (const auto* l = std::get_if<valkey::storage::List>(&v)) {
        return SizeOf(*l);
    }
    return SizeOf(std::get<valkey::storage::Set>(v));
}

constexpr bool IsEmpty(const valkey::storage::String&) {
    return false;
}

bool IsEmpty(const valkey::storage::List& lst) {
    return lst.empty();
}

bool IsEmpty(const valkey::storage::Set& s) {
    return s.empty();
}

class MemoryTracker {
public:
    void SetLimit(size_t bytes) { limit_ = bytes; }
    size_t Limit() const { return limit_; }
    size_t Used() const { return used_; }

    void Add(size_t bytes) { used_ += bytes; }
    void Sub(size_t bytes) { used_ -= std::min(bytes, used_); }

    bool WouldOverflow(size_t additional) const {
        return limit_ != 0 && used_ + additional > limit_;
    }

private:
    size_t used_ = 0;
    size_t limit_ = 0;
};

}  // namespace valkey::engine
