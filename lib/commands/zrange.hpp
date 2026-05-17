#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
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

namespace valkey::commands::zrange {

std::expected<parser::Cmd, parser::ParseError> Build(std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto lo = parser::TokenToInt(args[1], 2);
    if (!lo) {
        return std::unexpected(lo.error());
    }
    auto hi = parser::TokenToInt(args[2], 3);
    if (!hi) {
        return std::unexpected(hi.error());
    }
    bool ws = false;
    if (args.size() == 4) {
        auto opt = parser::TokenToString(args[3], 4);
        if (!opt) {
            return std::unexpected(opt.error());
        }
        if (!parser::CaseEqual(*opt, "WITHSCORES")) {
            return std::unexpected(parser::ParseError{"expected WITHSCORES", 4});
        }
        ws = true;
    }
    return cmd_nodes::ZRange{std::move(*k), *lo, *hi, ws};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::ZRange>(c);
    auto [z, st] = e.As<storage::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    const int64_t sz = static_cast<int64_t>(z->scores.size());
    int64_t lo = x.start;
    int64_t hi = x.stop;
    if (lo < 0) {
        lo = std::max(lo + sz, int64_t{0});
    }
    if (hi < 0) {
        hi = hi + sz;
    }
    if (hi >= sz) {
        hi = sz - 1;
    }
    if (lo > hi) {
        return io::MakeArray({});
    }
    auto items = z->sl->RangeByRank(static_cast<size_t>(lo), static_cast<size_t>(hi));
    std::vector<io::Reply> out;
    out.reserve(items.size() * (x.with_scores ? 2 : 1));
    for (const auto& [score, member] : items) {
        out.push_back(io::MakeBulk(member));
        if (x.with_scores) {
            out.push_back(io::MakeBulk(std::to_string(score)));
        }
    }
    return io::MakeArray(std::move(out));
}

struct Trait {
    using Node = cmd_nodes::ZRange;
    static constexpr parser::CommandEntry kEntry{"ZRANGE", {3, 4}, &Build, &Exec};
};

}  // namespace valkey::commands::zrange
