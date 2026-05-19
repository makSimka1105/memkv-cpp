#pragma once

#include <cstdint>
#include <deque>
#include <expected>
#include <span>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct LLen {
    std::string key;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::llen {

inline std::expected<cmd_nodes::LLen, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::LLen{std::move(*k)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::LLen& x) {
    auto [lst, st] = e.As<std::deque<std::string>>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    return io::MakeInt(lst->size());
}

using Trait = meta::CommandTrait<cmd_nodes::LLen, "LLEN", {1, 1}, &Build, &Exec>;

}  // namespace valkey::commands::llen
