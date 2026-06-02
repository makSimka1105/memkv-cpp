#pragma once
#include <cstddef>
#include <string_view>
namespace valkey {
template <size_t N>
struct FixedString {
    char data[N] = {};
    constexpr FixedString(const char (&s)[N]) {
        for (size_t i = 0; i < N; ++i) {
            data[i] = s[i];
        }
    }
    constexpr operator std::string_view() const { return {data, N - 1}; }
};

template <size_t N>
FixedString(const char (&)[N]) -> FixedString<N>;
}  // namespace valkey
