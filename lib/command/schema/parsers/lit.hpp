#pragma once
#include <cstddef>
#include <expected>
#include "meta/fixed_string.hpp"
#include "command/parser/cursor.hpp"
#include "command/diagnostic.hpp"
namespace valkey::schema {
template <FixedString Kw>
struct Lit {
    static constexpr size_t kMinArgc = 1;
    static constexpr size_t kMaxArgc = 1;
    template <class Node>
    static parser::CmdResult<void> Apply(parser::Cursor& cursor, Node&) {
        if (!cursor.MatchKeyword(Kw)) {
            return parser::err::SyntaxError();
        }
        return {};
    }
};
}  // namespace valkey::schema
