#pragma once

#include <chrono>
#include <expected>
#include <span>
#include <utility>
#include <variant>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::expire {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto secs = parser::TokenToInt(args[1], 2);
    if (!secs) {
        return std::unexpected(secs.error());
    }
    return cmd_nodes::Expire{std::move(*k), *secs};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::Expire>(c);
    bool ok = e.ExpireKey(x.key, std::chrono::seconds{x.seconds});
    return io::MakeInt(ok ? 1 : 0);
}

constexpr parser::CommandEntry kEntry{"EXPIRE", {2, 2}, &Build, &Exec};

}  // namespace valkey::commands::expire
