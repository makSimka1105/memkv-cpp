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

namespace valkey::commands::get {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::Get{std::move(*k)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::Get>(c);
    auto [s, st] = e.As<storage::String>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    return io::MakeBulk(*s);
}

constexpr parser::CommandEntry kEntry{"GET", {1, 1}, &Build, &Exec};

}  // namespace valkey::commands::get
