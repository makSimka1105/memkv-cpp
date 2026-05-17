#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <variant>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::dbsize {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> /*args*/) {
    return cmd_nodes::DBSize{};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& /*c*/) {
    return io::MakeInt(static_cast<int64_t>(e.DbSize()));
}

struct Trait {
    using Node = cmd_nodes::DBSize;
    static constexpr parser::CommandEntry kEntry{"DBSIZE", {0, 0}, &Build, &Exec};
};

}  // namespace valkey::commands::dbsize
