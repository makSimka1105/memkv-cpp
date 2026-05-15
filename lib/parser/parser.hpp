#pragma once

#include <expected>
#include <span>
#include <variant>

#include "parser/commands.hpp"
#include "parser/lexer/types.hpp"
#include "parser/registry.hpp"
#include "parser/token_extract.hpp"

namespace valkey::parser {

std::expected<Cmd, ParseError> Parse(std::span<const Token> tokens) {
    while (!tokens.empty() && std::holds_alternative<TkEof>(tokens.back())) {
        tokens = tokens.first(tokens.size() - 1);
    }

    if (tokens.empty()) {
        return std::unexpected(ParseError{"empty input", 0});
    }

    const auto* name = std::get_if<TkWord>(&tokens[0]);
    if (name == nullptr) {
        return std::unexpected(ParseError{"command name must be a word", 0});
    }

    const auto* entry = Lookup(name->text);
    if (entry == nullptr) {
        return std::unexpected(ParseError{"unknown command: " + name->text, 0});
    }

    const auto args = tokens.subspan(1);
    if (!ArityFits(entry->arity, args.size())) {
        return std::unexpected(ParseError{"wrong number of arguments for " + name->text, 0});
    }

    return entry->build(args);
}

}  // namespace valkey::parser
