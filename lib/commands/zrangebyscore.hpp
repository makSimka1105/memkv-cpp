#pragma once

#include <expected>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::zrangebyscore {

std::expected<parser::Cmd, parser::ParseError> Build(std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto mn = parser::TokenToFloat(args[1], 2);
    if (!mn) {
        return std::unexpected(mn.error());
    }
    auto mx = parser::TokenToFloat(args[2], 3);
    if (!mx) {
        return std::unexpected(mx.error());
    }
    return cmd_nodes::ZRangeByScore{std::move(*k), *mn, *mx};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::ZRangeByScore>(c);
    auto [z, st] = e.As<storage::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    auto items = z->sl->RangeByScore(x.min_s, x.max_s);
    std::vector<io::Reply> out;
    out.reserve(items.size());
    for (const auto& [_, member] : items) {
        out.push_back(io::MakeBulk(member));
    }
    return io::MakeArray(std::move(out));
}

struct Trait {
    using Node = cmd_nodes::ZRangeByScore;
    static constexpr parser::CommandEntry kEntry{"ZRANGEBYSCORE", {3, 3}, &Build, &Exec};
};

}  // namespace valkey::commands::zrangebyscore
