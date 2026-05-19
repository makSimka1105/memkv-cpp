#pragma once

#include <expected>
#include <span>
#include <string>
#include <utility>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct Type {
    std::string key;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::type {

inline std::expected<cmd_nodes::Type, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::Type{std::move(*k)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Type& x) {
    return io::SimpleStr{std::string(e.TypeOf(x.key))};
}

using Trait = meta::CommandTrait<cmd_nodes::Type, "TYPE", {1, 1}, &Build, &Exec>;

}  // namespace valkey::commands::type
