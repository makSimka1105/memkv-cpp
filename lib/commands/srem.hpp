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

struct SRem {
    std::string key;
    std::vector<std::string> members;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::srem {

inline std::expected<cmd_nodes::SRem, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto members = parser::CollectStrings(args.subspan(1), 2);
    if (!members) {
        return std::unexpected(members.error());
    }
    return cmd_nodes::SRem{std::move(*k), std::move(*members)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::SRem& x) {
    int64_t removed = 0;
    auto st = e.Mutate<std::unordered_set<std::string>>(x.key, [&](std::unordered_set<std::string>& s) {
        for (const auto& m : x.members) {
            removed += static_cast<int64_t>(s.erase(m));
        }
    });
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    return io::MakeInt(removed);
}

using Trait = meta::CommandTrait<cmd_nodes::SRem, "SREM", {2, parser::kUnbounded}, &Build, &Exec>;

}  // namespace valkey::commands::srem
