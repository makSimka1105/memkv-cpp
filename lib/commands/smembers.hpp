#pragma once

#include <expected>
#include <span>
#include <unordered_set>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct SMembers {
    std::string key;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::smembers {

inline std::expected<cmd_nodes::SMembers, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::SMembers{std::move(*k)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::SMembers& x) {
    auto [s, st] = e.As<std::unordered_set<std::string>>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    std::vector<io::Reply> items;
    items.reserve(s->size());
    for (const auto& m : *s) {
        items.push_back(io::MakeBulk(m));
    }
    return io::MakeArray(std::move(items));
}

using Trait = meta::CommandTrait<cmd_nodes::SMembers, "SMEMBERS", {1, 1}, &Build, &Exec>;

}  // namespace valkey::commands::smembers
