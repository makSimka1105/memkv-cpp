#pragma once

#include <expected>
#include <span>
#include <string>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"
#include "storage/value.hpp"

namespace valkey::commands::sunion {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto keys = parser::CollectStrings(args, 1);
    if (!keys) {
        return std::unexpected(keys.error());
    }
    return cmd_nodes::SUnion{std::move(*keys)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::SUnion>(c);
    std::unordered_set<std::string> result;
    for (const auto& key : x.keys) {
        auto [s, st] = e.As<storage::Set>(key);
        if (st == engine::Engine::TypeStatus::kWrongType) {
            return engine::WrongTypeReply();
        }
        if (s != nullptr) {
            result.insert(s->begin(), s->end());
        }
    }
    std::vector<io::Reply> items;
    items.reserve(result.size());
    for (const auto& m : result) {
        items.push_back(io::MakeBulk(m));
    }
    return io::MakeArray(std::move(items));
}

struct Trait {
    using Node = cmd_nodes::SUnion;
    static constexpr parser::CommandEntry kEntry{"SUNION", {1, parser::kUnbounded}, &Build, &Exec};
};

}  // namespace valkey::commands::sunion
