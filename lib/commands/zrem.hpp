#pragma once

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
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"
#include "structures/zset.hpp"

namespace valkey::cmd_nodes {

struct ZRem {
    std::string key;
    std::vector<std::string> members;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::zrem {

inline std::expected<cmd_nodes::ZRem, parser::ParseError> Build(std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto members = parser::CollectStrings(args.subspan(1), 2);
    if (!members) {
        return std::unexpected(members.error());
    }
    return cmd_nodes::ZRem{std::move(*k), std::move(*members)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::ZRem& x) {
    int64_t removed = 0;
    auto st = e.Mutate<structures::ZSet>(x.key, [&](structures::ZSet& z) {
        for (const auto& m : x.members) {
            auto it = z.scores.find(m);
            if (it != z.scores.end()) {
                z.sl->Erase(it->second, m);
                z.scores.erase(it);
                ++removed;
            }
        }
    });
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeInt(removed);
}

using Trait = meta::CommandTrait<cmd_nodes::ZRem, "ZREM", {2, parser::kUnbounded}, &Build, &Exec>;

}  // namespace valkey::commands::zrem
