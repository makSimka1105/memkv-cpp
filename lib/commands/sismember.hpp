#pragma once

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
#include "storage/value.hpp"

namespace valkey::commands::sismember {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto m = parser::TokenToString(args[1], 2);
    if (!m) {
        return std::unexpected(m.error());
    }
    return cmd_nodes::SIsMember{std::move(*k), std::move(*m)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::SIsMember>(c);
    auto [s, st] = e.As<storage::Set>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    return io::MakeInt(s->contains(x.member) ? 1 : 0);
}

struct Trait {
    using Node = cmd_nodes::SIsMember;
    static constexpr parser::CommandEntry kEntry{"SISMEMBER", {2, 2}, &Build, &Exec};
};

}  // namespace valkey::commands::sismember
