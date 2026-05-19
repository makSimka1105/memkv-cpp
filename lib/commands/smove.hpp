#pragma once

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

struct SMove {
    std::string src;
    std::string dst;
    std::string member;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::smove {

inline std::expected<cmd_nodes::SMove, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto src = parser::TokenToString(args[0], 1);
    if (!src) {
        return std::unexpected(src.error());
    }
    auto dst = parser::TokenToString(args[1], 2);
    if (!dst) {
        return std::unexpected(dst.error());
    }
    auto member = parser::TokenToString(args[2], 3);
    if (!member) {
        return std::unexpected(member.error());
    }
    return cmd_nodes::SMove{std::move(*src), std::move(*dst), std::move(*member)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::SMove& x) {
    auto [src_set, src_st] = e.As<std::unordered_set<std::string>>(x.src);
    if (src_st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (src_st == engine::Engine::TypeStatus::kMissing || !src_set->contains(x.member)) {
        return io::MakeInt(0);
    }
    auto [dst_set, dst_st] = e.As<std::unordered_set<std::string>>(x.dst);
    if (dst_st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    e.Mutate<std::unordered_set<std::string>>(x.src, [&](std::unordered_set<std::string>& s) { s.erase(x.member); });
    e.Upsert<std::unordered_set<std::string>>(x.dst, [&](std::unordered_set<std::string>& s) { s.insert(x.member); });
    return io::MakeInt(1);
}

using Trait = meta::CommandTrait<cmd_nodes::SMove, "SMOVE", {3, 3}, &Build, &Exec>;

}  // namespace valkey::commands::smove
