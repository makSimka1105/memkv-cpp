#pragma once

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

struct Get {
    std::string key;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::get {

inline std::expected<cmd_nodes::Get, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::Get{std::move(*k)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Get& x) {
    auto [s, st] = e.As<std::string>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    return io::MakeBulk(*s);
}

using Trait = meta::CommandTrait<cmd_nodes::Get, "GET", {1, 1}, &Build, &Exec>;

}  // namespace valkey::commands::get
