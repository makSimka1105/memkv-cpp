#pragma once

#include <expected>
#include <span>
#include <variant>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::flushdb {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> /*args*/) {
    return cmd_nodes::FlushDb{};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& /*c*/) {
    e.FlushDb();
    return io::Ok();
}

constexpr parser::CommandEntry kEntry{"FLUSHDB", {0, 0}, &Build, &Exec};

}  // namespace valkey::commands::flushdb
