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

struct Set {
    std::string key;
    std::string value;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::set {

inline std::expected<cmd_nodes::Set, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto v = parser::TokenToString(args[1], 2);
    if (!v) {
        return std::unexpected(v.error());
    }
    return cmd_nodes::Set{std::move(*k), std::move(*v)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Set& x) {
    if (!e.SetString(x.key, x.value)) {
        return engine::OomReply();
    }
    return io::Ok();
}

using Trait = meta::CommandTrait<cmd_nodes::Set, "SET", {2, 2}, &Build, &Exec>;

}  // namespace valkey::commands::set
