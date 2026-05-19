#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"
#include "structures/zset.hpp"

namespace valkey::cmd_nodes {

struct ZRank {
    std::string key;
    std::string member;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::zrank {

inline std::expected<cmd_nodes::ZRank, parser::ParseError> Build(std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto m = parser::TokenToString(args[1], 2);
    if (!m) {
        return std::unexpected(m.error());
    }
    return cmd_nodes::ZRank{std::move(*k), std::move(*m)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::ZRank& x) {
    auto [z, st] = e.As<structures::ZSet>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    auto it = z->scores.find(x.member);
    if (it == z->scores.end()) {
        return io::MakeNil();
    }
    auto r = z->sl->Rank(it->second, x.member);
    if (!r) {
        return io::MakeNil();
    }
    return io::MakeInt(static_cast<int64_t>(*r));
}

using Trait = meta::CommandTrait<cmd_nodes::ZRank, "ZRANK", {2, 2}, &Build, &Exec>;

}  // namespace valkey::commands::zrank
