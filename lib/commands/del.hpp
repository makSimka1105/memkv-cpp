#pragma once

#include <cstdint>
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

namespace valkey::commands::del {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto keys = parser::CollectStrings(args, 1);
    if (!keys) {
        return std::unexpected(keys.error());
    }
    return cmd_nodes::Del{std::move(*keys)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::Del>(c);
    int64_t count = 0;
    for (const auto& key : x.keys) {
        if (e.DelKey(key)) {
            ++count;
        }
    }
    return io::MakeInt(count);
}

constexpr parser::CommandEntry kEntry{"DEL", {1, parser::kUnbounded}, &Build, &Exec};

}  // namespace valkey::commands::del
