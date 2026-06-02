#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string_view>

#include "lexer/types.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/combinators/seq.hpp"
#include "schema/core/concepts.hpp"
#include "util/str.hpp"

namespace valkey::schema {

template <class Node, FieldFor<Node>... Fields>
struct Schema {
    using NodeType = Node;
    using Body = Seq<Node, Fields...>;

    static constexpr size_t MinArgc() { return Body::kMinArgc; }
    static constexpr size_t MaxArgc() { return Body::kMaxArgc; }

    static std::expected<Node, parser::ParseError> Parse(std::span<const parser::Token> args,
                                                         std::string_view cmd_name = "") {
        if (args.size() < MinArgc() || args.size() > MaxArgc()) {
            return std::unexpected(parser::ParseError{
                "ERR wrong number of arguments for '" + LowerName(cmd_name) + "' command", 0});
        }
        parser::Cursor cursor{args, 0};
        auto result = Body::Parse(cursor);
        if (!result) {
            return std::unexpected(result.error());
        }
        if (!cursor.Done()) {
            return std::unexpected(parser::ParseError{"ERR syntax error", cursor.Pos()});
        }
        return std::move(*result);
    }
};

}  // namespace valkey::schema
