#pragma once

#include <cstddef>
#include <expected>
#include <optional>
#include <utility>

#include "command/parser/cursor.hpp"
#include "command/schema/parsers/scan.hpp"
#include "command/diagnostic.hpp"

namespace valkey::schema {

template <class Type>
struct Optional {
    using Output = Type;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static parser::CmdResult<Type> Parse(parser::CmdTokenCursor& cursor) {
        if (cursor.AtEnd()) {
            return Type{};
        }
        auto sp = cursor.Save();
        auto result = ScanValue<Type>(cursor);
        if (!result) {
            return Type{};
        }
        sp.Commit();
        return std::move(*result);
    }
};

template <class Type>
struct Optional<std::optional<Type>> {
    using Output = std::optional<Type>;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static parser::CmdResult<std::optional<Type>> Parse(parser::CmdTokenCursor& cursor) {
        if (cursor.AtEnd()) {
            return std::nullopt;
        }
        auto sp = cursor.Save();
        auto result = ScanValue<Type>(cursor);
        if (!result) {
            return std::nullopt;
        }
        sp.Commit();
        return std::move(*result);
    }
};

}  // namespace valkey::schema
