#pragma once

#include <expected>
#include <span>
#include <string>
#include <utility>
#include <variant>

#include "engine/config.hpp"
#include "engine/engine.hpp"
#include "engine/replies.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry_types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::commands::config {

std::expected<parser::Cmd, parser::ParseError> Build(
    std::span<const parser::Token> args) {
    auto sub = parser::TokenToString(args[0], 1);
    if (!sub) {
        return std::unexpected(sub.error());
    }
    auto param = parser::TokenToString(args[1], 2);
    if (!param) {
        return std::unexpected(param.error());
    }
    if (parser::CaseEqual(*sub, "GET")) {
        return cmd_nodes::Config{false, std::move(*param), {}};
    }
    if (parser::CaseEqual(*sub, "SET")) {
        if (args.size() < 3) {
            return std::unexpected(
                parser::ParseError{"CONFIG SET requires a value", 3});
        }
        auto val = parser::TokenToString(args[2], 3);
        if (!val) {
            return std::unexpected(val.error());
        }
        return cmd_nodes::Config{true, std::move(*param), std::move(*val)};
    }
    return std::unexpected(parser::ParseError{"unknown CONFIG subcommand: " + *sub, 1});
}

io::Reply Exec(engine::Engine& e, const parser::Cmd& c) {
    const auto& x = std::get<cmd_nodes::Config>(c);
    if (!parser::CaseEqual(x.param, "maxmemory")) {
        return io::MakeErr("ERR unknown option: " + x.param);
    }
    if (!x.is_set) {
        std::vector<io::Reply> arr;
        arr.push_back(io::MakeBulk("maxmemory"));
        arr.push_back(io::MakeBulk(std::to_string(e.MaxMemory())));
        return io::MakeArray(std::move(arr));
    }
    auto bytes = engine::ParseMemorySize(x.value);
    if (!bytes) {
        return io::MakeErr("ERR invalid value for maxmemory: " + x.value);
    }
    e.SetMaxMemory(*bytes);
    return io::Ok();
}

constexpr parser::CommandEntry kEntry{"CONFIG", {2, 3}, &Build, &Exec};

}  // namespace valkey::commands::config
