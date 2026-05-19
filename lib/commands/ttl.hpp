#pragma once

#include <expected>
#include <span>
#include <utility>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct Ttl {
    std::string key;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::ttl {

inline std::expected<cmd_nodes::Ttl, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::Ttl{std::move(*k)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Ttl& x) {
    return io::MakeInt(e.TtlSeconds(x.key));
}

using Trait = meta::CommandTrait<cmd_nodes::Ttl, "TTL", {1, 1}, &Build, &Exec>;

}  // namespace valkey::commands::ttl
