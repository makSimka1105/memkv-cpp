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

namespace valkey::commands::append {

std::expected<parser::Cmd, parser::ParseError> Build(
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

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::Append>(c);
    int64_t length = 0;
    auto st = e.Upsert<storage::String>(x.key, [&](storage::String& s) {
        s.append(x.value);
        length = s.size();
    });
    switch (st) {
        case engine::Engine::UpsertStatus::kWrongType:
            return engine::WrongTypeReply();
        case engine::Engine::UpsertStatus::kOom:
            return engine::OomReply();
        case engine::Engine::UpsertStatus::kOk:
            return io::MakeInt(length);
    }
    return engine::WrongTypeReply();
}

constexpr parser::CommandEntry kEntry{"APPEND", {2, 2}, &Build, &Exec};

}  // namespace valkey::commands::append
