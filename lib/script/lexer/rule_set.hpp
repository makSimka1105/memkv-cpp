#pragma once

#include <expected>
#include <optional>

#include "script/error.hpp"
#include "script/token.hpp"
#include "script/lexer/cursor.hpp"

namespace valkey::script {

using MatchResult = std::optional<std::expected<Token, ScriptError>>;

template <typename... Rules>
struct RuleSet;

template <typename First, typename... Rest>
struct RuleSet<First, Rest...> {
    static std::expected<Token, ScriptError> Try(Cursor& c) {
        if (auto r = First::Match(c)) return *r;
        return RuleSet<Rest...>::Try(c);
    }
};

template <>
struct RuleSet<> {
    static std::expected<Token, ScriptError> Try(Cursor& c) {
        return MakeError(ErrKind::kLex,
                         std::string("unexpected character: ") + c.Peek());
    }
};

}  // namespace valkey::script
