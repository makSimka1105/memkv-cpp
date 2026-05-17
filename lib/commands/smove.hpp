#pragma once

#include <expected>
#include <span>
#include <utility>
#include <variant>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"
#include "storage/value.hpp"

namespace valkey::commands::smove {

std::expected<parser::Cmd, parser::ParseError> Build(
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

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::SMove>(c);
    auto [src_set, src_st] = e.As<storage::Set>(x.src);
    if (src_st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (src_st == engine::Engine::TypeStatus::kMissing || !src_set->contains(x.member)) {
        return io::MakeInt(0);
    }
    auto [dst_set, dst_st] = e.As<storage::Set>(x.dst);
    if (dst_st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    e.Mutate<storage::Set>(x.src, [&](storage::Set& s) { s.erase(x.member); });
    e.Upsert<storage::Set>(x.dst, [&](storage::Set& s) { s.insert(x.member); });
    return io::MakeInt(1);
}

struct Trait {
    using Node = cmd_nodes::SMove;
    static constexpr parser::CommandEntry kEntry{"SMOVE", {3, 3}, &Build, &Exec};
};

}  // namespace valkey::commands::smove
