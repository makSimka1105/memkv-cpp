#pragma once

#include <expected>
#include <span>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct Keys {
    std::string pattern;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::keys {

inline std::expected<cmd_nodes::Keys, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto pattern = parser::TokenToString(args[0], 1);
    if (!pattern) {
        return std::unexpected(pattern.error());
    }
    return cmd_nodes::Keys{std::move(*pattern)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Keys& x) {
    auto matched = e.Keys(x.pattern);
    std::vector<io::Reply> items;
    items.reserve(matched.size());
    for (auto& k : matched) {
        items.push_back(io::MakeBulk(std::move(k)));
    }
    return io::MakeArray(std::move(items));
}

using Trait = meta::CommandTrait<cmd_nodes::Keys, "KEYS", {1, 1}, &Build, &Exec>;

}  // namespace valkey::commands::keys
