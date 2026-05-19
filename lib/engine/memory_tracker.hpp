#pragma once

#include <algorithm>
#include <cstddef>

#include "meta/type_list.hpp"
#include "stored_types/all.hpp"

namespace valkey::engine {

template <class T>
inline size_t SizeOf(const T& val) {
    return meta::TraitForT<T, stored_types::AllStoredTypes>::SizeOf(val);
}

template <class T>
inline bool IsEmpty(const T& val) {
    return meta::TraitForT<T, stored_types::AllStoredTypes>::IsEmpty(val);
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
