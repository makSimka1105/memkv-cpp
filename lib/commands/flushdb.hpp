#pragma once

#include <expected>
#include <span>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct FlushDb {};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::flushdb {

inline std::expected<cmd_nodes::FlushDb, parser::ParseError> Build(
    std::span<const parser::Token> /*args*/) {
    return cmd_nodes::FlushDb{};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::FlushDb& /*x*/) {
    e.FlushDb();
    return io::Ok();
}

using Trait = meta::CommandTrait<cmd_nodes::FlushDb, "FLUSHDB", {0, 0}, &Build, &Exec>;

}  // namespace valkey::commands::flushdb
