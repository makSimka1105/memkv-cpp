#pragma once

#include <expected>
#include <span>
#include <string>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"
#include "meta/command_trait.hpp"
#include "structures/zset.hpp"

namespace valkey::cmd_nodes {

struct ZIncrBy {
    std::string key;
    double delta;
    std::string member;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::zincrby {

inline std::expected<cmd_nodes::ZIncrBy, parser::ParseError> Build(std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto d = parser::TokenToFloat(args[1], 2);
    if (!d) {
        return std::unexpected(d.error());
    }
    auto m = parser::TokenToString(args[2], 3);
    if (!m) {
        return std::unexpected(m.error());
    }
    return cmd_nodes::ZIncrBy{std::move(*k), *d, std::move(*m)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::ZIncrBy& x) {
    double new_score = 0;
    auto st = e.Upsert<structures::ZSet>(x.key, [&](structures::ZSet& z) {
        auto it = z.scores.find(x.member);
        if (it == z.scores.end()) {
            new_score = x.delta;
            z.sl->Insert(new_score, x.member);
            z.scores.emplace(x.member, new_score);
        } else {
            z.sl->Erase(it->second, x.member);
            new_score = it->second + x.delta;
            z.sl->Insert(new_score, x.member);
            it->second = new_score;
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
    return io::MakeBulk(std::to_string(new_score));
}

using Trait = meta::CommandTrait<cmd_nodes::ZIncrBy, "ZINCRBY", {3, 3}, &Build, &Exec>;

}  // namespace valkey::commands::zincrby
