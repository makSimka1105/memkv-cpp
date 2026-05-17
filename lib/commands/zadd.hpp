#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::zadd {

std::expected<parser::Cmd, parser::ParseError> Build(std::span<const parser::Token> args) {
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

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::ZAdd>(c);
    int64_t added = 0;
    auto st = e.Upsert<storage::ZSet>(x.key, [&](storage::ZSet& z) {
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
        case engine::Engine::UpsertStatus::kOk:
            return io::MakeInt(added);
    }
    __builtin_unreachable();
}

struct Trait {
    using Node = cmd_nodes::ZAdd;
    static constexpr parser::CommandEntry kEntry{"ZADD", {3, parser::kUnbounded}, &Build, &Exec};
};

}  // namespace valkey::commands::zadd
