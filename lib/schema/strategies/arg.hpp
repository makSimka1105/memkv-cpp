#pragma once

#include <cstddef>
#include <expected>
#include <optional>

#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/scan.hpp"

namespace valkey::schema {

template <class T>
struct Arg {
    using Output = T;
    static constexpr size_t kMinArgc = 1;
    static constexpr size_t kMaxArgc = 1;

    static std::expected<T, parser::ParseError> Parse(parser::Cursor& c) { return ScanValue<T>(c); }
};

template <class T>
struct OptArg {
    using Output = T;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static std::expected<T, parser::ParseError> Parse(parser::Cursor& c) {
        if (c.Done()) {
            return T{};
        }
        size_t saved = c.pos;
        auto r = ScanValue<T>(c);
        if (!r) {
            c.pos = saved;
            return T{};
        }
        return *r;
    }
};

template <class T>
struct OptArg<std::optional<T>> {
    using Output = std::optional<T>;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static std::expected<std::optional<T>, parser::ParseError> Parse(parser::Cursor& c) {
        if (c.Done()) {
            return std::nullopt;
        }
        auto r = ScanValue<T>(c);
        if (!r) {
            return std::nullopt;
        }
        return *r;
    }
};

}  // namespace valkey::schema
