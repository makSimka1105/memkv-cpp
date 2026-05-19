#pragma once

#include <cstddef>
#include <limits>
#include <string_view>

namespace valkey::parser {

struct ArityRange {
    size_t min;
    size_t max;
};

constexpr size_t kUnbounded = std::numeric_limits<size_t>::max();

}  // namespace valkey::parser

namespace valkey::meta {

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

template <class T, FixedString Name, parser::ArityRange Arity, auto BuildFn, auto ExecFn>
struct CommandTrait {
    using Type = T;
    static constexpr std::string_view kName = Name;
    static constexpr parser::ArityRange kArity = Arity;
    static constexpr auto kBuildFn = BuildFn;
    static constexpr auto kExecFn = ExecFn;
};

}  // namespace valkey::meta
