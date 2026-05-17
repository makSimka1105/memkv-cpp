#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"
#include "storage/value.hpp"

namespace valkey::commands::sinter {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto keys = parser::CollectStrings(args, 1);
    if (!keys) {
        return std::unexpected(keys.error());
    }
    return cmd_nodes::SInter{std::move(*keys)};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::SInter>(c);
    if (x.keys.empty()) {
        return io::MakeArray({});
    }
    auto [first, st0] = e.As<storage::Set>(x.keys[0]);
    if (st0 == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (st0 == engine::Engine::TypeStatus::kMissing) {
        return io::MakeArray({});
    }
    std::unordered_set<std::string> result(first->begin(), first->end());
    for (size_t i = 1; i < x.keys.size(); ++i) {
        auto [s, st] = e.As<storage::Set>(x.keys[i]);
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

struct Trait {
    using Node = cmd_nodes::SInter;
    static constexpr parser::CommandEntry kEntry{"SINTER", {1, parser::kUnbounded}, &Build, &Exec};
};

}  // namespace valkey::commands::sinter
