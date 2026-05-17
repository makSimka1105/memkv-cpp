#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <string>
#include <string_view>
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

namespace valkey::commands::rpop {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto k = parser::TokenToString(args[0], 1);
    if (!k) {
        return std::unexpected(k.error());
    }
    std::optional<int64_t> count;
    if (args.size() == 2) {
        auto n = parser::TokenToInt(args[1], 2);
        if (!n) {
            return std::unexpected(n.error());
        }
        count = *n;
    }
    return cmd_nodes::RPop{std::move(*k), count};
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::RPop>(c);
    const int64_t want = x.count.value_or(1);
    if (want < 0) {
        return io::MakeErr("ERR value is not an integer or out of range");
    }
    std::vector<std::string> out;
    auto st = e.Mutate<storage::List>(x.key, [&](storage::List& lst) {
        const int64_t n = std::min(want, static_cast<int64_t>(lst.size()));
        out.reserve(static_cast<size_t>(n));
        for (int64_t i = 0; i < n; ++i) {
            out.push_back(std::move(lst.back()));
            lst.pop_back();
        }
    });
    if (st == engine::Engine::TypeStatus::kWrongType) {
        return engine::WrongTypeReply();
    }
    if (!x.count.has_value()) {
        if (out.empty()) {
            return io::MakeNil();
        }
        return io::MakeBulk(std::move(out[0]));
    }
    std::vector<io::Reply> items;
    items.reserve(out.size());
    for (auto& s : out) {
        items.push_back(io::MakeBulk(std::move(s)));
    }
    return io::MakeArray(std::move(items));
}

struct Trait {
    using Node = cmd_nodes::RPop;
    static constexpr parser::CommandEntry kEntry{"RPOP", {1, 2}, &Build, &Exec};
};

}  // namespace valkey::commands::rpop
