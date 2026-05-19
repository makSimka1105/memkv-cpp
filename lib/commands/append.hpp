#pragma once

#include <cstdint>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct Append {
    std::string key;
    std::string value;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::append {

inline std::expected<cmd_nodes::Append, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    auto v = parser::TokenToString(args[1], 2);
    if (!v) {
        return std::unexpected(v.error());
    }
    return cmd_nodes::Append{std::move(*k), std::move(*v)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::Append& x) {
    int64_t length = 0;
    auto st = e.Upsert<std::string>(x.key, [&](std::string& s) {
        s.append(x.value);
        length = s.size();
    });
    switch (st) {
        case engine::Engine::UpsertStatus::kWrongType:
            return engine::WrongTypeReply();
        case engine::Engine::UpsertStatus::kOom:
            return engine::OomReply();
        default:
            break;
    }
    return io::MakeInt(length);
}

using Trait = meta::CommandTrait<cmd_nodes::Append, "APPEND", {2, 2}, &Build, &Exec>;

}  // namespace valkey::commands::append
