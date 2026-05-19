#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <utility>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct Exists {
    std::vector<std::string> keys;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::exists {

inline std::expected<cmd_nodes::Exists, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto keys = parser::CollectStrings(args, 1);
    if (!keys) {
        return std::unexpected(keys.error());
    }
    return cmd_nodes::Exists{std::move(*keys)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Exists& x) {
    int64_t count = 0;
    for (const auto& key : x.keys) {
        if (e.ExistsKey(key)) {
            ++count;
        }
    }
    return io::MakeInt(count);
}

using Trait = meta::CommandTrait<cmd_nodes::Exists, "EXISTS", {1, parser::kUnbounded}, &Build, &Exec>;

}  // namespace valkey::commands::exists
