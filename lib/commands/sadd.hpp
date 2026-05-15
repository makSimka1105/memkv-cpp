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

namespace valkey::commands::sadd {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto members = parser::CollectStrings(args.subspan(1), 2);
    if (!members) {
        return std::unexpected(members.error());
    }
    return cmd_nodes::SAdd{std::move(*k), std::move(*members)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::SAdd>(c);
    int64_t added = 0;
    auto st = e.Upsert<storage::Set>(x.key, [&](storage::Set& s) {
        for (const auto& m : x.members) {
            if (s.insert(m).second) {
                ++added;
            }
        }
    });
    switch (st) {
        case engine::Engine::UpsertStatus::kWrongType:
            return engine::WrongTypeReply();
        case engine::Engine::UpsertStatus::kOom:
            return engine::OomReply();
        case engine::Engine::UpsertStatus::kOk:
            return io::MakeInt(added);
    }
    return engine::WrongTypeReply();
}

constexpr parser::CommandEntry kEntry{"SADD", {2, parser::kUnbounded}, &Build, &Exec};

}  // namespace valkey::commands::sadd
