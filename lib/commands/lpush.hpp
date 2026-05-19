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

struct LPush {
    std::string key;
    std::vector<std::string> values;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::lpush {

inline std::expected<cmd_nodes::LPush, parser::ParseError> Build(
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

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::LPush& x) {
    int64_t length = 0;
    auto st = e.Upsert<std::deque<std::string>>(x.key, [&](std::deque<std::string>& lst) {
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
        default:
            break;
    }
    return io::MakeInt(length);
}

using Trait = meta::CommandTrait<cmd_nodes::LPush, "LPUSH", {2, parser::kUnbounded}, &Build, &Exec>;

}  // namespace valkey::commands::lpush
