#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::zcard {

std::expected<parser::Cmd, parser::ParseError> Build(std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::ZCard{std::move(*k)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::ZCard>(c);
    auto [z, st] = e.As<storage::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    return io::MakeInt(static_cast<int64_t>(z->scores.size()));
}

struct Trait {
    using Node = cmd_nodes::ZCard;
    static constexpr parser::CommandEntry kEntry{"ZCARD", {1, 1}, &Build, &Exec};
};

}  // namespace valkey::commands::zcard
