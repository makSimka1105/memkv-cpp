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

namespace valkey::commands::lpush {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto vals = parser::CollectStrings(args.subspan(1), 2);
    if (!vals) {
        return std::unexpected(vals.error());
    }
    return cmd_nodes::LPush{std::move(*k), std::move(*vals)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::LPush>(c);
    int64_t length = 0;
    auto st = e.Upsert<storage::List>(x.key, [&](storage::List& lst) {
        for (const auto& s : x.values) {
            lst.push_front(s);
        }
        length = lst.size();
    });
    switch (st) {
        case engine::Engine::UpsertStatus::kWrongType:
            return engine::WrongTypeReply();
        case engine::Engine::UpsertStatus::kOom:
            return engine::OomReply();
        case engine::Engine::UpsertStatus::kOk:
            return io::MakeInt(length);
    }
    return engine::WrongTypeReply();
}

constexpr parser::CommandEntry kEntry{"LPUSH", {2, parser::kUnbounded}, &Build, &Exec};

}  // namespace valkey::commands::lpush
