#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string_view>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct DBSize {};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::dbsize {

inline std::expected<cmd_nodes::DBSize, parser::ParseError> Build(std::span<const parser::Token> /*args*/) {
    return cmd_nodes::DBSize{};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::DBSize& /*x*/) {
    return io::MakeInt(static_cast<int64_t>(e.DbSize()));
}

using Trait = meta::CommandTrait<cmd_nodes::DBSize, "DBSIZE", {0, 0}, &Build, &Exec>;

}  // namespace valkey::commands::dbsize
