#pragma once

#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <variant>

#include "command/commands/all.hpp"
#include "command/lexer/types.hpp"
#include "meta/type_list.hpp"
#include "command/parser/cmd.hpp"
#include "command/diagnostic.hpp"
#include "util/str.hpp"

namespace valkey::parser {

namespace detail {

template <class... Traits>
CmdResult<Cmd> DispatchList(std::span<const Token>, std::string_view,
                            std::string_view, TraitsList<Traits...>);

template <class Trait>
CmdResult<Cmd> DispatchNode(std::span<const Token> tokens,
                            std::string_view full_name) {
    if constexpr (!Trait::kIsGroup) {
        return Trait::SchemaType::Parse(tokens, full_name);
    } else {
        if (tokens.empty()) {
            return full_name.empty() ? err::EmptyInput() : err::WrongArgs(full_name);
        }
        const auto* sub_name = std::get_if<TkWord>(&tokens[0]);
        if (!sub_name) {
            return err::SyntaxError();
        }
        return DispatchList(tokens.subspan(1), sub_name->text, full_name,
                            typename Trait::ChildrenList{});
    }
}

template <class... Traits>
CmdResult<Cmd> DispatchList(std::span<const Token> tokens, std::string_view word,
                            std::string_view full_prefix, TraitsList<Traits...>) {
    CmdResult<Cmd> result =
        full_prefix.empty() ? err::UnknownCommand(word) : err::UnknownSubcommand(word);

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

CmdResult<Cmd> Parse(std::span<const Token> tokens) {
    while (!tokens.empty() && std::holds_alternative<TkEof>(tokens.back())) {
        tokens = tokens.first(tokens.size() - 1);
    }
    return detail::DispatchNode<commands::RootGroup>(tokens, "");
}

}  // namespace valkey::parser
