#pragma once

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

namespace valkey::commands::lset {

std::expected<parser::Cmd, parser::ParseError> Build(
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

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::LSet>(c);
    bool out_of_range = false;
    auto st = e.Mutate<storage::List>(x.key, [&](storage::List& lst) {
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

constexpr parser::CommandEntry kEntry{"LSET", {3, 3}, &Build, &Exec};

}  // namespace valkey::commands::lset
