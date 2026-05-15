#pragma once

#include <expected>
#include <span>
#include <string>
#include <utility>
#include <variant>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::type {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::Type{std::move(*k)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::Type>(c);
    return io::SimpleStr{std::string(e.TypeOf(x.key))};
}

constexpr parser::CommandEntry kEntry{"TYPE", {1, 1}, &Build, &Exec};

}  // namespace valkey::commands::type
