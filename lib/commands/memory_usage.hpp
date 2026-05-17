#pragma once

#include <cstdint>
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

namespace valkey::commands::memory_usage {

std::expected<parser::Cmd, parser::ParseError> Build(
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

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::Memory>(c);
    size_t bytes = e.MemoryUsage(x.key);
    if (bytes == 0) {
        return io::MakeNil();
    }
    return io::MakeInt(bytes);
}

struct Trait {
    using Node = cmd_nodes::Memory;
    static constexpr parser::CommandEntry kEntry{"MEMORY", {2, 2}, &Build, &Exec};
};

}  // namespace valkey::commands::memory_usage
