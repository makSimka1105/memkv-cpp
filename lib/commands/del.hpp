#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "lexer/types.hpp"
#include "parser/token_convert.hpp"

namespace valkey::cmd_nodes {

struct Del {
    std::vector<std::string> keys;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::del {

inline std::expected<cmd_nodes::Del, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto keys = parser::CollectStrings(args, 1);
    if (!keys) {
        return std::unexpected(keys.error());
    }
    return cmd_nodes::Del{std::move(*keys)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Del& x) {
    int64_t count = 0;
    for (const auto& key : x.keys) {
        if (e.DelKey(key)) {
            ++count;
        }
    }
    return io::MakeInt(count);
}

using Trait = meta::CommandTrait<cmd_nodes::Del, "DEL", {1, parser::kUnbounded},
                                  &Build, &Exec>;

}  // namespace valkey::commands::del
