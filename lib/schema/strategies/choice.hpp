#pragma once

#include <cstddef>
#include <expected>
#include <optional>
#include <string_view>

#include "schema/command_trait.hpp"
#include "parser/cursor.hpp"
#include "parser/error.hpp"
#include "parser/token_convert.hpp"

namespace valkey::schema {

template <FixedString A, FixedString B>
struct Choice {
    using Output = bool;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static std::expected<bool, parser::ParseError> Parse(parser::Cursor& c) {
        if (c.MatchKeyword(A)) {
            return true;
        }
        if (c.MatchKeyword(B)) {
            return false;
        }
        return false;
    }

    static constexpr bool MatchesKeyword(std::string_view word) {
        return parser::CaseEqual(word, A) || parser::CaseEqual(word, B);
    }

    static constexpr bool Default() { return false; }
};

template <FixedString A, FixedString B>
struct OptionalChoice {
    using Output = std::optional<bool>;
    static constexpr size_t kMinArgc = 0;
    static constexpr size_t kMaxArgc = 1;

    static std::expected<std::optional<bool>, parser::ParseError> Parse(parser::Cursor& c) {
        if (c.MatchKeyword(A)) {
            return true;
        }
        if (c.MatchKeyword(B)) {
            return false;
        }
        return std::nullopt;
    }

    static constexpr bool MatchesKeyword(std::string_view word) {
        return parser::CaseEqual(word, A) || parser::CaseEqual(word, B);
    }

    static std::optional<bool> Default() { return std::nullopt; }
};

}  // namespace valkey::schema
