#pragma once

#include <expected>
#include <span>
#include <utility>
#include <variant>
#include <vector>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::keys {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto pattern = parser::TokenToString(args[0], 1);
    if (!pattern) {
        return std::unexpected(pattern.error());
    }
    return cmd_nodes::Keys{std::move(*pattern)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::Keys>(c);
    auto matched = e.Keys(x.pattern);
    std::vector<io::Reply> items;
    items.reserve(matched.size());
    for (auto& k : matched) {
        items.push_back(io::MakeBulk(std::move(k)));
    }
    return io::MakeArray(std::move(items));
}

constexpr parser::CommandEntry kEntry{"KEYS", {1, 1}, &Build, &Exec};

}  // namespace valkey::commands::keys
