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
#include "storage/value.hpp"

namespace valkey::commands::srem {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto members = parser::CollectStrings(args.subspan(1), 2);
    if (!members) {
        return std::unexpected(members.error());
    }
    return cmd_nodes::SRem{std::move(*k), std::move(*members)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::SRem>(c);
    int64_t removed = 0;
    auto st = e.Mutate<storage::Set>(x.key, [&](storage::Set& s) {
        for (const auto& m : x.members) {
            removed += static_cast<int64_t>(s.erase(m));
        }
    });
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeInt(removed);
}

constexpr parser::CommandEntry kEntry{"SREM", {2, parser::kUnbounded}, &Build, &Exec};

}  // namespace valkey::commands::srem
