#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string_view>

#include "lexer/types.hpp"
#include "parser/cursor.hpp"
#include "diagnostic.hpp"
#include "meta/try.hpp"
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

    static parser::CmdResult<Node> Parse(std::span<const parser::Token> args,
                                         std::string_view cmd_name = "") {
        if (args.size() < MinArgc() || args.size() > MaxArgc()) {
            return parser::err::WrongArgs(cmd_name);
        }
        parser::Cursor cursor{args};
        VK_ASSIGN_OR_RETURN(auto node, Body::Parse(cursor));
        if (!cursor.Done()) {
            return parser::err::SyntaxError();
        }
        return node;
    }
};

}  // namespace valkey::schema
