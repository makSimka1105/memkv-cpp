#pragma once
#include <cstddef>
#include <limits>
namespace valkey::schema {
constexpr size_t kUnbounded = std::numeric_limits<size_t>::max();
constexpr size_t SaturatingAdd(size_t a, size_t b) {
    if (a == kUnbounded || b == kUnbounded) {
        return kUnbounded;
    }
    if (a > kUnbounded - b) {
        return kUnbounded;
    }
    return a + b;
}
template <class... Values>
constexpr size_t SaturatingSum(Values... values) {
    size_t total = 0;
    ((total = SaturatingAdd(total, values)), ...);
    return total;
}
}  // namespace valkey::schema
