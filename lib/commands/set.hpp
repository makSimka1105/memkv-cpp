#pragma once

#include <expected>
#include <span>
#include <utility>
#include <variant>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::set {

std::expected<parser::Cmd, parser::ParseError> Build(
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

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::Set>(c);
    if (!e.SetString(x.key, x.value)) {
        return engine::OomReply();
    }
    return io::Ok();
}

struct Trait {
    using Node = cmd_nodes::Set;
    static constexpr parser::CommandEntry kEntry{"SET", {2, 2}, &Build, &Exec};
};

}  // namespace valkey::commands::set
