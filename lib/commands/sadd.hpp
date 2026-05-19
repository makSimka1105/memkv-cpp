#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <unordered_set>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct SAdd {
    std::string key;
    std::vector<std::string> members;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::sadd {

inline std::expected<cmd_nodes::SAdd, parser::ParseError> Build(
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

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::SAdd& x) {
    int64_t added = 0;
    auto st = e.Upsert<std::unordered_set<std::string>>(x.key, [&](std::unordered_set<std::string>& s) {
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
        default:
            break;
    }
    return io::MakeInt(added);
}

using Trait = meta::CommandTrait<cmd_nodes::SAdd, "SADD", {2, parser::kUnbounded}, &Build, &Exec>;

}  // namespace valkey::commands::sadd
