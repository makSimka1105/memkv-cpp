#pragma once

#include <algorithm>
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

namespace valkey::commands::linsert {

std::expected<parser::Cmd, parser::ParseError> Build(
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

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::LInsert>(c);
    int64_t result = 0;
    bool pivot_found = false;
    auto st = e.Mutate<storage::List>(x.key, [&](storage::List& lst) {
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

struct Trait {
    using Node = cmd_nodes::LInsert;
    static constexpr parser::CommandEntry kEntry{"LINSERT", {4, 4}, &Build, &Exec};
};

}  // namespace valkey::commands::linsert
