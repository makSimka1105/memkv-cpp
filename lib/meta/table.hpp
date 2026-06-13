#pragma once

#include <array>
#include <cstddef>

namespace valkey {

template <class T, size_t N>
class DispatchTable {
 public:
    constexpr void Set(size_t index, T value) {
        values_[index] = value;
        filled_[index] = true;
    }

    constexpr bool Complete() const {
        for (bool filled : filled_) {
            if (!filled) {
                return false;
            }
        }
        return true;
    }

    constexpr T operator[](size_t index) const { return values_[index]; }

 private:
    std::array<T, N> values_{};
    std::array<bool, N> filled_{};
};

}  // namespace valkey
