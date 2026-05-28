#pragma once

#include <cstddef>
#include <expected>

#include "schema/command_trait.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"

namespace valkey::schema {

template <FixedString Kw>
struct LiteralRule {
    static constexpr size_t kMinArgc = 1;
    static constexpr size_t kMaxArgc = 1;

    template <class Node>
    static std::expected<void, parser::ParseError> Apply(parser::Cursor& c, Node&) {
        if (!c.MatchKeyword(Kw)) {
            return std::unexpected(parser::ParseError{"ERR syntax error", c.Pos()});
        }
        return {};
    }
};

}  // namespace valkey::schema
