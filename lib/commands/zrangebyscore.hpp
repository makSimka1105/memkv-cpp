#pragma once

#include <expected>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"
#include "structures/zset.hpp"

namespace valkey::cmd_nodes {

struct ZRangeByScore {
    std::string key;
    double min_s;
    double max_s;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::zrangebyscore {

inline std::expected<cmd_nodes::ZRangeByScore, parser::ParseError>
Build(std::span<const parser::Token> args) {
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

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::ZRangeByScore& x) {
    auto [z, st] = e.As<structures::ZSet>(x.key);
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

using Trait = meta::CommandTrait<cmd_nodes::ZRangeByScore, "ZRANGEBYSCORE", parser::ArityRange{3, 3}, &Build, &Exec>;

}  // namespace valkey::commands::zrangebyscore
