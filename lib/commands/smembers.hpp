#pragma once

#include <expected>
#include <span>
#include <utility>
#include <variant>
#include <vector>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"
#include "storage/value.hpp"

namespace valkey::commands::smembers {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    return cmd_nodes::SMembers{std::move(*k)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::SMembers>(c);
    auto [s, st] = e.As<storage::Set>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    std::vector<io::Reply> items;
    items.reserve(s->size());
    for (const auto& m : *s) {
        items.push_back(io::MakeBulk(m));
    }
    return io::MakeArray(std::move(items));
}

struct Trait {
    using Node = cmd_nodes::SMembers;
    static constexpr parser::CommandEntry kEntry{"SMEMBERS", {1, 1}, &Build, &Exec};
};

}  // namespace valkey::commands::smembers
