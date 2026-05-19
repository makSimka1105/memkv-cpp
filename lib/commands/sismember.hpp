#pragma once

#include <expected>
#include <span>
#include <unordered_set>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct SIsMember {
    std::string key;
    std::string member;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::sismember {

inline std::expected<cmd_nodes::SIsMember, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto m = parser::TokenToString(args[1], 2);
    if (!m) {
        return std::unexpected(m.error());
    }
    return cmd_nodes::SIsMember{std::move(*k), std::move(*m)};
}

inline io::Reply Exec(engine::Engine& e,
                      const cmd_nodes::SIsMember& x) {
    auto [s, st] = e.As<std::unordered_set<std::string>>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    return io::MakeInt(s->contains(x.member) ? 1 : 0);
}

using Trait = meta::CommandTrait<cmd_nodes::SIsMember, "SISMEMBER", {2, 2},
                                  &Build, &Exec>;

}  // namespace valkey::commands::sismember
