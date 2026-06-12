#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string_view>

#include "command/lexer/types.hpp"
#include "command/parser/cursor.hpp"
#include "command/schema/combinators/seq.hpp"
#include "command/schema/core/concepts.hpp"
#include "command/diagnostic.hpp"
#include "meta/try.hpp"
#include "util/str.hpp"

namespace valkey::schema {

template <class Node, FieldFor<Node>... Fields>
struct Schema {
    using NodeType = Node;
    using Body = Seq<Node, Fields...>;

    static constexpr size_t MinArgc() { return Body::kMinArgc; }
    static constexpr size_t MaxArgc() { return Body::kMaxArgc; }

    static parser::CmdResult<Node> Parse(std::span<const parser::Token> args,
                                         std::string_view cmd_name = "") {
        if (args.size() < MinArgc() || args.size() > MaxArgc()) {
            return parser::err::WrongArgs(cmd_name);
        }
        parser::CmdTokenCursor cursor{args};
        VK_ASSIGN_OR_RETURN(auto node, Body::Parse(cursor));
        if (!cursor.AtEnd()) {
            return parser::err::SyntaxError();
        }
        return node;
    }
};

}  // namespace valkey::schema
