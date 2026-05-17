#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::zrem {

std::expected<parser::Cmd, parser::ParseError> Build(std::span<const parser::Token> args) {
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

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::ZRem>(c);
    int64_t removed = 0;
    auto st = e.Mutate<storage::ZSet>(x.key, [&](storage::ZSet& z) {
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

struct Trait {
    using Node = cmd_nodes::ZRem;
    static constexpr parser::CommandEntry kEntry{"ZREM", {2, parser::kUnbounded}, &Build, &Exec};
};

}  // namespace valkey::commands::zrem
