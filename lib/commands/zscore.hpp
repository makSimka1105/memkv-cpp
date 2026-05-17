#pragma once

#include <expected>
#include <span>
#include <string>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::zscore {

std::expected<parser::Cmd, parser::ParseError> Build(std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto m = parser::TokenToString(args[1], 2);
    if (!m) {
        return std::unexpected(m.error());
    }
    return cmd_nodes::ZScore{std::move(*k), std::move(*m)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::ZScore>(c);
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
    return io::MakeBulk(std::to_string(it->second));
}

struct Trait {
    using Node = cmd_nodes::ZScore;
    static constexpr parser::CommandEntry kEntry{"ZSCORE", {2, 2}, &Build, &Exec};
};

}  // namespace valkey::commands::zscore
