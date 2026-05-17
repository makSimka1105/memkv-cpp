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

namespace valkey::commands::sdiff {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto keys = parser::CollectStrings(args, 1);
    if (!keys) {
        return std::unexpected(keys.error());
    }
    return cmd_nodes::SDiff{std::move(*keys)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::SDiff>(c);
    if (x.keys.empty()) {
        return io::MakeArray({});
    }
    auto [first, st0] = e.As<storage::Set>(x.keys[0]);
    if (st0 == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    std::unordered_set<std::string> result;
    if (first != nullptr) {
        result.insert(first->begin(), first->end());
    }
    for (size_t i = 1; i < x.keys.size(); ++i) {
        auto [s, st] = e.As<storage::Set>(x.keys[i]);
        if (st == engine::Engine::TypeStatus::kWrongType) {
            return engine::WrongTypeReply();
        }
        if (s != nullptr) {
            for (const auto& m : *s) {
                result.erase(m);
            }
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
    using Node = cmd_nodes::SDiff;
    static constexpr parser::CommandEntry kEntry{"SDIFF", {1, parser::kUnbounded}, &Build, &Exec};
};

}  // namespace valkey::commands::sdiff
