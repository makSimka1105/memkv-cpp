#pragma once

#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <variant>

#include "commands/all.hpp"
#include "lexer/types.hpp"
#include "meta/type_list.hpp"
#include "parser/cmd.hpp"
#include "parser/error.hpp"
#include "parser/token_convert.hpp"

namespace valkey::parser {

namespace detail {

template <class... Traits>
std::expected<Cmd, ParseError> DispatchList(std::span<const Token>, std::string_view,
                                            std::string_view, TraitsList<Traits...>);

template <class Trait>
std::expected<Cmd, ParseError> DispatchNode(std::span<const Token> tokens,
                                            std::string_view full_name) {
    if constexpr (!Trait::kIsGroup) {
        return Trait::SchemaType::Parse(tokens, full_name);
    } else {
        if (tokens.empty()) {
            if (full_name.empty()) {
                return std::unexpected(ParseError{"ERR empty input", 0});
            }
            std::string lower(full_name);
            for (char& c : lower) {
                if (c >= 'A' && c <= 'Z') {
                    c = static_cast<char>(c - 'A' + 'a');
                }
            }
            return std::unexpected(
                ParseError{"ERR wrong number of arguments for '" + lower + "' command", 0});
        }
        const auto* sub = std::get_if<TkWord>(&tokens[0]);
        if (!sub) {
            return std::unexpected(ParseError{"ERR syntax error", 0});
        }
        return DispatchList(tokens.subspan(1), sub->text, full_name,
                            typename Trait::ChildrenList{});
    }
}

template <class... Traits>
std::expected<Cmd, ParseError> DispatchList(std::span<const Token> tokens, std::string_view word,
                                            std::string_view full_prefix,
                                            TraitsList<Traits...>) {
    std::expected<Cmd, ParseError> result = std::unexpected(ParseError{
        (full_prefix.empty() ? "ERR unknown command '" : "ERR unknown subcommand '") +
            std::string(word) + "'",
        0});

    ((CaseEqual(Traits::kName, word) &&
      [&] {
          std::string full_name = full_prefix.empty()
                                      ? std::string(Traits::kName)
                                      : std::string(full_prefix) + " " + std::string(Traits::kName);
          result = DispatchNode<Traits>(tokens, full_name);
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
    return detail::DispatchNode<commands::RootGroup>(tokens, "");
}

}  // namespace valkey::parser
