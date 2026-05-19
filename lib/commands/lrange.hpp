#pragma once

#include <algorithm>
#include <cstdint>
#include <deque>
#include <expected>
#include <span>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct LRange {
    std::string key;
    int64_t start;
    int64_t stop;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::lrange {

inline std::expected<cmd_nodes::LRange, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto start = parser::TokenToInt(args[1], 2);
    if (!start) {
        return std::unexpected(start.error());
    }
    auto stop = parser::TokenToInt(args[2], 3);
    if (!stop) {
        return std::unexpected(stop.error());
    }
    return cmd_nodes::LRange{std::move(*k), *start, *stop};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::LRange& x) {
    auto [lst, st] = e.As<std::deque<std::string>>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    int64_t start = x.start;
    int64_t stop = x.stop;
    const int64_t sz = lst->size();
    if (start < 0) {
        start = std::max(start + sz, int64_t{0});
    }
    if (stop < 0) {
        stop = stop + sz;
    }
    if (stop >= sz) {
        stop = sz - 1;
    }
    if (start > stop) {
        return io::MakeArray({});
    }
    std::vector<io::Reply> items;
    items.reserve(stop - start + 1);
    for (int64_t i = start; i <= stop; ++i) {
        items.push_back(io::MakeBulk((*lst)[i]));
    }
    return io::MakeArray(std::move(items));
}

using Trait = meta::CommandTrait<cmd_nodes::LRange, "LRANGE", {3, 3}, &Build, &Exec>;

}  // namespace valkey::commands::lrange
