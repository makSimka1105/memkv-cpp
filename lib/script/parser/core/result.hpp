#pragma once

#include <concepts>
#include <expected>
#include <variant>

#include "script/error.hpp"
#include "script/token.hpp"

namespace valkey::script {

class ScriptTokenCursor;

template <typename T>
using ParseResult = std::expected<T, ScriptError>;

bool TokenIsKw(const Token& tok, TkKw::Keyword kw) {
    const auto* k = std::get_if<TkKw>(&tok);
    return k != nullptr && k->kw == kw;
}

namespace detail {
struct ParserProbe;
}  // namespace detail

template <typename T>
concept Dispatchable =
    requires(const Token& tok, ScriptTokenCursor& cursor, detail::ParserProbe& parser) {
        { T::Matches(tok) } -> std::convertible_to<bool>;
        T::Parse(cursor, parser);
    };

}  // namespace valkey::script
