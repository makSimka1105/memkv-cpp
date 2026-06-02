#pragma once
#include <cstddef>
#include <expected>
#include "meta/fixed_string.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
namespace valkey::schema {
template <FixedString Kw>
struct Lit {
    static constexpr size_t kMinArgc = 1;
    static constexpr size_t kMaxArgc = 1;
    template <class Node>
    static std::expected<void, parser::ParseError> Apply(parser::Cursor& cursor, Node&) {
        if (!cursor.MatchKeyword(Kw)) {
            return std::unexpected(parser::ParseError{"ERR syntax error", cursor.Pos()});
        }
        return {};
    }
};
}  // namespace valkey::schema
