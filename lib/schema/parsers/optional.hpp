#pragma once

#include <cstddef>
#include <expected>
#include <optional>

#include "parser/cursor.hpp"
#include "parser/errors.hpp"
#include "schema/parsers/scan.hpp"

namespace valkey::schema {

template <class Type>
struct Optional {
    using Output = Type;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static std::expected<Type, parser::ParseError> Parse(parser::Cursor& cursor) {
        if (cursor.Done()) {
            return Type{};
        }
        size_t saved_pos = cursor.Pos();
        auto result = ScanValue<Type>(cursor);
        if (!result) {
            cursor.Restore(saved_pos);
            return Type{};
        }
        return *result;
    }
};

template <class Type>
struct Optional<std::optional<Type>> {
    using Output = std::optional<Type>;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static std::expected<std::optional<Type>, parser::ParseError> Parse(parser::Cursor& cursor) {
        if (cursor.Done()) {
            return std::nullopt;
        }
        size_t saved_pos = cursor.Pos();
        auto result = ScanValue<Type>(cursor);
        if (!result) {
            cursor.Restore(saved_pos);
            return std::nullopt;
        }
        return *result;
    }
};

}  // namespace valkey::schema
