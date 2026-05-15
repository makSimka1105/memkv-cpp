#pragma once

#include <cstddef>
#include <expected>
#include <limits>
#include <span>
#include <string_view>

#include "engine/engine.hpp"
#include "io/reply.hpp"
#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::parser {

struct ArityRange {
    size_t min;
    size_t max;
};

constexpr size_t kUnbounded = std::numeric_limits<size_t>::max();

using BuildFn = std::expected<Cmd, ParseError> (*)(std::span<const Token>);
using ExecFn = io::Reply (*)(engine::Engine&, const Cmd&);

struct CommandEntry {
    std::string_view name;
    ArityRange arity;
    BuildFn build;
    ExecFn exec;
};

}  // namespace valkey::parser
