#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"
#include "structures/zset.hpp"

namespace valkey::cmd_nodes {

struct ZCard {
    std::string key;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::zcard {

inline std::expected<cmd_nodes::ZCard, parser::ParseError> Build(std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::ZCard{std::move(*k)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::ZCard& x) {
    auto [z, st] = e.As<structures::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    return io::MakeInt(static_cast<int64_t>(z->scores.size()));
}

using Trait = meta::CommandTrait<cmd_nodes::ZCard, "ZCARD", {1, 1}, &Build, &Exec>;

}  // namespace valkey::commands::zcard
