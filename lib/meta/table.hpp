#pragma once

#include <array>
#include <cstddef>

namespace valkey {

template <class T, size_t N>
constexpr bool AllSet(const std::array<T, N>& table) {
    for (const auto& entry : table) {
        if (!entry) {
            return false;
        }
    }
    return true;
}

}  // namespace valkey
