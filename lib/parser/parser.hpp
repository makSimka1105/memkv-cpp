#pragma once

#include <expected>
#include <span>
#include <string>
#include <variant>

#include "commands/all.hpp"
#include "meta/type_list.hpp"
#include "parser/cmd.hpp"
#include "parser/lexer/types.hpp"
#include "parser/token_extract.hpp"

namespace valkey::parser {

namespace detail {

template <class... Traits>
std::expected<Cmd, ParseError> ParseImpl(std::string_view name, std::span<const Token> args,
                                          meta::TraitsList<Traits...>) {
    std::expected<Cmd, ParseError> result =
        std::unexpected(ParseError{"unknown command: " + std::string(name), 0});

    ((CaseEqual(Traits::kName, name) &&
      [&] {
          if (args.size() < Traits::kArity.min || args.size() > Traits::kArity.max) {
              result = std::unexpected(
                  ParseError{"wrong number of arguments for " + std::string(name), 0});
          } else {
              auto r = Traits::kBuildFn(args);
              if (r) {
                  result = Cmd{std::move(*r)};
              } else {
                  result = std::unexpected(r.error());
              }
          }
          return true;
      }()) ||
      ...);

    return result;
}

}  // namespace detail

std::expected<Cmd, ParseError> Parse(std::span<const Token> tokens) {
    while (!tokens.empty() && std::holds_alternative<TkEof>(tokens.back())) {
        tokens = tokens.first(tokens.size() - 1);
    }

    if (tokens.empty()) {
        return std::unexpected(ParseError{"empty input", 0});
    }

    const auto* name_tok = std::get_if<TkWord>(&tokens[0]);
    if (name_tok == nullptr) {
        return std::unexpected(ParseError{"command name must be a word", 0});
    }

    return detail::ParseImpl(name_tok->text, tokens.subspan(1), commands::AllCommandsTraits{});
}

}  // namespace valkey::parser
