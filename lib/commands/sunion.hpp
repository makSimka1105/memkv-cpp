#pragma once

#include <expected>
#include <span>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "meta/command_trait.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::cmd_nodes {

struct SUnion {
    std::vector<std::string> keys;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::sunion {

inline std::expected<cmd_nodes::SUnion, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto keys = parser::CollectStrings(args, 1);
    if (!keys) {
        return std::unexpected(keys.error());
    }
    return cmd_nodes::SUnion{std::move(*keys)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::SUnion& x) {
    std::unordered_set<std::string> result;
    for (const auto& key : x.keys) {
        auto [s, st] = e.As<std::unordered_set<std::string>>(key);
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

using Trait = meta::CommandTrait<cmd_nodes::SUnion, "SUNION", {1, parser::kUnbounded}, &Build, &Exec>;

}  // namespace valkey::commands::sunion
