#pragma once

#include <cstdint>
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

struct Memory {
    std::string subcommand;
    std::string key;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::memory_usage {

inline std::expected<cmd_nodes::Memory, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto sub = parser::TokenToString(args[0], 1);
    if (!sub) {
        return std::unexpected(sub.error());
    }
    if (!parser::CaseEqual(*sub, "USAGE")) {
        return std::unexpected(
            parser::ParseError{"unknown MEMORY subcommand: " + *sub, 1});
    }
    auto k = parser::TokenToString(args[1], 2);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::Memory{"USAGE", std::move(*k)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Memory& x) {
    size_t bytes = e.MemoryUsage(x.key);
    if (bytes == 0) {
        return io::MakeNil();
    }
    return io::MakeInt(bytes);
}

using Trait = meta::CommandTrait<cmd_nodes::Memory, "MEMORY", {2, 2}, &Build, &Exec>;

}  // namespace valkey::commands::memory_usage
