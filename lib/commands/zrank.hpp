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

namespace valkey::commands::zrank {

std::expected<parser::Cmd, parser::ParseError> Build(std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto m = parser::TokenToString(args[1], 2);
    if (!m) {
        return std::unexpected(m.error());
    }
    return cmd_nodes::ZRank{std::move(*k), std::move(*m)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::ZRank>(c);
    auto [z, st] = e.As<storage::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    auto it = z->scores.find(x.member);
    if (it == z->scores.end()) {
        return io::MakeNil();
    }
    auto r = z->sl->Rank(it->second, x.member);
    if (!r) {
        return io::MakeNil();
    }
    return io::MakeInt(static_cast<int64_t>(*r));
}

struct Trait {
    using Node = cmd_nodes::ZRank;
    static constexpr parser::CommandEntry kEntry{"ZRANK", {2, 2}, &Build, &Exec};
};

}  // namespace valkey::commands::zrank
