#pragma once

#include <algorithm>
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

struct LInsert {
    std::string key;
    bool before;
    std::string pivot;
    std::string value;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::linsert {

inline std::expected<cmd_nodes::LInsert, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto dir = parser::TokenToString(args[1], 2);
    if (!dir) {
        return std::unexpected(dir.error());
    }
    bool before = false;
    if (parser::CaseEqual(*dir, "BEFORE")) {
        before = true;
    } else if (parser::CaseEqual(*dir, "AFTER")) {
        before = false;
    } else {
        return std::unexpected(parser::ParseError{"syntax error, expected BEFORE or AFTER", 2});
    }
    auto pivot = parser::TokenToString(args[2], 3);
    if (!pivot) {
        return std::unexpected(pivot.error());
    }
    auto val = parser::TokenToString(args[3], 4);
    if (!val) {
        return std::unexpected(val.error());
    }
    return cmd_nodes::LInsert{std::move(*k), before, std::move(*pivot), std::move(*val)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::LInsert& x) {
    int64_t result = 0;
    bool pivot_found = false;
    auto st = e.Mutate<std::deque<std::string>>(x.key, [&](std::deque<std::string>& lst) {
        auto it = std::find(lst.begin(), lst.end(), x.pivot);
        if (it == lst.end()) {
            return;
        }
        pivot_found = true;
        if (!x.before) {
            ++it;
        }
        lst.insert(it, x.value);
        result = lst.size();
    });
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeInt(0);
    }
    if (!pivot_found) {
        return io::MakeInt(-1);
    }
    return io::MakeInt(result);
}

using Trait = meta::CommandTrait<cmd_nodes::LInsert, "LINSERT", {4, 4}, &Build, &Exec>;

}  // namespace valkey::commands::linsert
