#pragma once

#include <cstdint>
#include <deque>
#include <expected>
#include <span>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct LIndex {
    std::string key;
    int64_t index;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::lindex {

inline std::expected<cmd_nodes::LIndex, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto idx = parser::TokenToInt(args[1], 2);
    if (!idx) {
        return std::unexpected(idx.error());
    }
    return cmd_nodes::LIndex{std::move(*k), *idx};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::LIndex& x) {
    auto [lst, st] = e.As<std::deque<std::string>>(x.key);
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st == engine::Engine::TypeStatus::kMissing) {
        return io::MakeNil();
    }
    int64_t index = x.index;
    const int64_t sz = lst->size();
    if (index < 0) {
        index += sz;
    }
    if (index < 0 || index >= sz) {
        return io::MakeNil();
    }
    return io::MakeBulk((*lst)[index]);
}

using Trait = meta::CommandTrait<cmd_nodes::LIndex, "LINDEX", {2, 2}, &Build, &Exec>;

}  // namespace valkey::commands::lindex
