#pragma once

#include <cstdint>
#include <deque>
#include <expected>
#include <span>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct LSet {
    std::string key;
    int64_t index;
    std::string value;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::lset {

inline std::expected<cmd_nodes::LSet, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto idx = parser::TokenToInt(args[1], 2);
    if (!idx) {
        return std::unexpected(idx.error());
    }
    auto v = parser::TokenToString(args[2], 3);
    if (!v) {
        return std::unexpected(v.error());
    }
    return cmd_nodes::LSet{std::move(*k), *idx, std::move(*v)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::LSet& x) {
    bool out_of_range = false;
    auto st = e.Mutate<std::deque<std::string>>(x.key, [&](std::deque<std::string>& lst) {
        int64_t index = x.index;
        const int64_t sz = lst.size();
        if (index < 0) {
            index += sz;
        }
        if (index < 0 || index >= sz) {
            out_of_range = true;
            return;
        }
        lst[index] = x.value;
    });
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing || out_of_range) {
        return io::MakeErr("ERR index out of range");
    }
    return io::Ok();
}

using Trait = meta::CommandTrait<cmd_nodes::LSet, "LSET", {3, 3}, &Build, &Exec>;

}  // namespace valkey::commands::lset
