#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <utility>
// #include <variant>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"
#include "storage/value.hpp"

namespace valkey::commands::lindex {

std::expected<parser::Cmd, parser::ParseError> Build(
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

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::LIndex>(c);
    auto [lst, st] = e.As<storage::List>(x.key);
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

struct Trait {
    using Node = cmd_nodes::LIndex;
    static constexpr parser::CommandEntry kEntry{"LINDEX", {2, 2}, &Build, &Exec};
};

}  // namespace valkey::commands::lindex
