#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <string_view>

#include "lexer/types.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "schema/apply.hpp"
#include "schema/contract.hpp"

namespace valkey::schema {

template <class Node, RuleFor<Node>... Rules>
struct Schema {
    using NodeType = Node;

    static constexpr size_t MinArgc() { return SaturatingSum(Rules::kMinArgc...); }
    static constexpr size_t MaxArgc() { return SaturatingSum(Rules::kMaxArgc...); }

    static std::expected<Node, parser::ParseError> Parse(std::span<const parser::Token> args,
                                                         std::string_view cmd_name = "") {
        if (args.size() < MinArgc() || args.size() > MaxArgc()) {
            std::string name;
            name.reserve(cmd_name.size());
            for (char ch : cmd_name) {
                if (ch >= 'A' && ch <= 'Z') {
                    ch = static_cast<char>(ch - 'A' + 'a');
                }
                name.push_back(ch);
            }
            return std::unexpected(
                parser::ParseError{"ERR wrong number of arguments for '" + name + "' command", 0});
        }
        Node node{};
        parser::Cursor c{args, 0};
        auto r = ApplyRules<Rules...>(c, node);
        if (!r) {
            return std::unexpected(r.error());
        }
        if (!c.Done()) {
            return std::unexpected(parser::ParseError{"ERR syntax error", c.Pos()});
        }
        return node;
    }
};

}  // namespace valkey::schema
