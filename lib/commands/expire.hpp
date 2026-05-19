#pragma once

#include <chrono>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct Expire {
    std::string key;
    int64_t seconds;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::expire {

inline std::expected<cmd_nodes::Expire, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto secs = parser::TokenToInt(args[1], 2);
    if (!secs) {
        return std::unexpected(secs.error());
    }
    return cmd_nodes::Expire{std::move(*k), *secs};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Expire& x) {
    bool ok = e.ExpireKey(x.key, std::chrono::seconds{x.seconds});
    return io::MakeInt(ok ? 1 : 0);
}

using Trait = meta::CommandTrait<cmd_nodes::Expire, "EXPIRE", {2, 2}, &Build, &Exec>;

}  // namespace valkey::commands::expire
