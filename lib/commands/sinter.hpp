#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <string_view>
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

struct SInter {
    std::vector<std::string> keys;
};

}  // namespace valkey::cmd_nodes

namespace valkey::commands::sinter {

inline std::expected<cmd_nodes::SInter, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto keys = parser::CollectStrings(args, 1);
    if (!keys) {
        return std::unexpected(keys.error());
    }
    return cmd_nodes::SInter{std::move(*keys)};
}

inline io::Reply Exec(engine::Engine& e, const cmd_nodes::SInter& x) {
    if (x.keys.empty()) {
        return io::MakeArray({});
    }
    auto [first, st0] = e.As<std::unordered_set<std::string>>(x.keys[0]);
    if (st0 == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st0 == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    std::unordered_set<std::string> result(first->begin(), first->end());
    for (size_t i = 1; i < x.keys.size(); ++i) {
        auto [s, st] = e.As<std::unordered_set<std::string>>(x.keys[i]);
        if (st == engine::Engine::TypeStatus::kWrongType) {
            return engine::WrongTypeReply();
        }
        std::unordered_set<std::string> next;
        if (s != nullptr) {
            for (const auto& m : result) {
                if (s->contains(m)) {
                    next.insert(m);
                }
            }
        }
        result = std::move(next);
        if (result.empty()) {
            break;
        }
    }
    std::vector<io::Reply> items;
    items.reserve(result.size());
    for (const auto& m : result) {
        items.push_back(io::MakeBulk(m));
    }
    return io::MakeArray(std::move(items));
}

using Trait = meta::CommandTrait<cmd_nodes::SInter, "SINTER", {1, parser::kUnbounded}, &Build, &Exec>;

}  // namespace valkey::commands::sinter
