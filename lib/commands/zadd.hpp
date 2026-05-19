#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"
#include "meta/command_trait.hpp"
#include "structures/zset.hpp"

namespace valkey::cmd_nodes {

struct ZAdd {
    std::string key;
    std::vector<std::pair<double, std::string>> entries;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::zadd {

inline std::expected<cmd_nodes::ZAdd, parser::ParseError> Build(std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto rest = args.subspan(1);
    if (rest.size() % 2 != 0) {
        return std::unexpected(parser::ParseError{"ZADD expects score-member pairs", 2});
    }
    std::vector<std::pair<double, std::string>> entries;
    entries.reserve(rest.size() / 2);
    for (size_t i = 0; i < rest.size(); i += 2) {
        auto sc = parser::TokenToFloat(rest[i], 2 + i);
        if (!sc) {
            return std::unexpected(sc.error());
        }
        auto m = parser::TokenToString(rest[i + 1], 3 + i);
        if (!m) {
            return std::unexpected(m.error());
        }
        entries.emplace_back(*sc, std::move(*m));
    }
    return cmd_nodes::ZAdd{std::move(*k), std::move(entries)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::ZAdd& x) {
    int64_t added = 0;
    auto st = e.Upsert<structures::ZSet>(x.key, [&](structures::ZSet& z) {
        for (const auto& [score, member] : x.entries) {
            auto it = z.scores.find(member);
            if (it == z.scores.end()) {
                z.sl->Insert(score, member);
                z.scores.emplace(member, score);
                ++added;
            } else if (it->second != score) {
                z.sl->Erase(it->second, member);
                z.sl->Insert(score, member);
                it->second = score;
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

using Trait = meta::CommandTrait<cmd_nodes::ZAdd, "ZADD", {3, parser::kUnbounded}, &Build, &Exec>;

}  // namespace valkey::commands::zadd
