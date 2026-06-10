#pragma once

#include <expected>
#include <optional>
#include <string>
#include <utility>

#include "meta/match_first.hpp"
#include "script/error.hpp"
#include "script/token.hpp"
#include "script/lexer/cursor.hpp"

namespace valkey::script {

using MatchResult = std::optional<std::expected<Token, ScriptError>>;

template <typename... Rules>
struct RuleSet {
    static std::expected<Token, ScriptError> Try(Cursor& c) {
        std::optional<std::expected<Token, ScriptError>> hit;
        MatchFirst<Rules...>([&]<size_t, typename R>() {
            if (auto r = R::Match(c)) {
                hit = std::move(*r);
                return true;
            }
            return false;
        });
        if (hit) {
            return std::move(*hit);
        }
        return MakeError(ErrKind::kLex, std::string("unexpected character: ") + c.Peek());
    }
};

}  // namespace valkey::script
